#include "GameLogic.h"
#include <windows.h>
#include <string>
#include <random>
#include <algorithm>
#include <sstream>
#include <tchar.h>

#include <windows.h>
#include <commctrl.h>
#include "GameLogic.h"

// Make sure to link with Comctl32.lib for ListView control
#pragma comment(lib, "comctl32.lib")

// Control Identifiers
#define ID_LISTVIEW 101
#define ID_GUESSBUTTON 102
#define ID_INPUT 103

// Global variables
HWND hListView;
HWND hGuessButton;
HWND hInput;
std::vector<Player> players;
Player targetPlayer;

// Forward declarations
void InitializeGame();
void AddListViewItems(const Player& guessedPlayer, const Player& targetPlayer);
void UpdateListView(HWND listView, const Player& guessedPlayer, const Player& targetPlayer);
void SetupListViewColumns(HWND listView);

// Window procedure for the main window
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            // Initialize the game data
            InitializeGame();

            // Create input box for player name guesses
            hInput = CreateWindow("EDIT", "",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER,
                                  10, 10, 180, 20, hwnd, (HMENU)ID_INPUT, NULL, NULL);

            // Create Guess button
            hGuessButton = CreateWindow("BUTTON", "Guess",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        200, 10, 80, 20, hwnd, (HMENU)ID_GUESSBUTTON, NULL, NULL);

            // Create ListView for displaying guesses
            hListView = CreateWindow(WC_LISTVIEW, "",
                                     WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                     10, 40, 400, 200, hwnd, (HMENU)ID_LISTVIEW, NULL, NULL);

            // Setup columns in the ListView
            SetupListViewColumns(hListView);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_GUESSBUTTON) {
                // When the Guess button is clicked
                char buffer[256];
                GetWindowText(hInput, buffer, 256);
                std::string guess(buffer);

                // Use the game logic to check the guess and update the ListView
                for (auto& player : players) {
                    if (guess == player.name) {
                        // Update the ListView with the guess results
                        UpdateListView(hListView, player, targetPlayer);
                        break;
                    }
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Function to set up columns in the ListView
void SetupListViewColumns(HWND listView) {

    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvColumn.cx = 120; // Width of the column

    // Add "Attribute" column
    lvColumn.pszText = _T("Attribute");
    ListView_InsertColumn(listView, 0, &lvColumn);

    // Add "Guessed" column
    lvColumn.pszText = "Guessed";
    ListView_InsertColumn(listView, 1, &lvColumn);

    // Add "Result" column
    lvColumn.pszText = "Result";
    ListView_InsertColumn(listView, 2, &lvColumn);
}

// Function to add items to the ListView based on the player's guess and the actual player data
void UpdateListView(HWND listView, const Player& guessedPlayer, const Player& targetPlayer) {
    // Clear previous items
    ListView_DeleteAllItems(listView);

    // Set up the list view item structure.
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;

    // Buffers for text
    char textBuffer[256];
    char resultBuffer[256];

    // Attributes to be checked
    const char* attributes[] = {"Team", "Conference", "Position", "Height", "Age", "Number"};
    const std::string* guessedAttributes[] = {
            &guessedPlayer.team, &guessedPlayer.conference, &guessedPlayer.position,
            &guessedPlayer.height, &guessedPlayer.age, &guessedPlayer.number
    };
    const std::string* actualAttributes[] = {
            &targetPlayer.team, &targetPlayer.conference, &targetPlayer.position,
            &targetPlayer.height, &targetPlayer.age, &targetPlayer.number
    };

    // Insert items and subitems for each attribute
    for (int i = 0; i < 6; ++i) {
        // Set attribute name
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = const_cast<LPSTR>(attributes[i]);
        ListView_InsertItem(listView, &lvItem);

        // Set guessed attribute
        strcpy_s(textBuffer, guessedAttributes[i]->c_str());
        ListView_SetItemText(listView, i, 1, textBuffer);

        // Set result based on comparison
        const char* resultText = guessedAttributes[i]->compare(*actualAttributes[i]) == 0 ? "Match" : "No match";
        strcpy_s(resultBuffer, resultText);
        ListView_SetItemText(listView, i, 2, resultBuffer);
    }

    // Force the ListView to update
    InvalidateRect(listView, NULL, TRUE);
    UpdateWindow(listView);
}



// Entry point function
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    const wchar_t CLASS_NAME[] = L"NBA Wordle Game Window";

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInst;
    wc.lpszClassName = reinterpret_cast<LPCSTR>(CLASS_NAME);
    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(0, reinterpret_cast<LPCSTR>(CLASS_NAME), reinterpret_cast<LPCSTR>(L"NBA Wordle"), WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 420, 300,
                               NULL, NULL, hInst, NULL);
    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, ncmdshow);
    UpdateWindow(hwnd);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

// Initialize the game by loading player data and selecting a random target player
void InitializeGame() {
    // Load player data into the players vector
    players = parseAndStorePlayerData();

    // Check if player data is available
    if (players.empty()) {
        MessageBox(NULL, reinterpret_cast<LPCSTR>(L"No player data available."), reinterpret_cast<LPCSTR>(L"Error"), MB_OK | MB_ICONERROR);
        ExitProcess(1);
    }

    // Select a random target player from the list
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> uni(0, players.size() - 1);
    targetPlayer = players[uni(rng)];
}
