#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_QUESTIONS 50
#define GAME_QUESTIONS 10
#define NAME_LENGTH 50
#define LEADERBOARD_FILE "leaderboard.txt"
#define SAVED_GAME_FILE "saved_game.txt"
#define QUESTIONS_FILE "questions.txt"

typedef struct {
    char question[256];
    char options[4][100];
    int correctAnswer;
    int difficulty;
} Question;

void showMenu();
void startGame();
void resumeGame();
void showLeaderboard();
void saveScore(const char *name, int score);
void loadQuestions(Question questions[]);
void shuffleQuestions(Question questions[], int count);
void askQuestion(Question *q, int *score);
void saveGame(const char *playerName, int questionIndex, int score);
int loadSavedGame(char *playerName, int *questionIndex, int *score);

void showMenu() {
    printf("\n===== TRIVIA GAME =====");
    printf("\n1. Start New Game");
    printf("\n2. Resume Game");
    printf("\n3. View Leaderboard");
    printf("\n4. Add Question");
    printf("\n5. Exit");
    printf("\n=======================");
    printf("\nChoose an option: ");
}

void loadQuestions(Question questions[]) {
    FILE *file = fopen(QUESTIONS_FILE, "r");
    if (file == NULL) {
        printf("\nError opening questions file.\n");
        exit(1);
    }

    int i = 0;
    while (fscanf(file, "%255[^|]|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%d|%d\n",
                   questions[i].question,
                   questions[i].options[0],
                   questions[i].options[1],
                   questions[i].options[2],
                   questions[i].options[3],
                   &questions[i].correctAnswer,
                   &questions[i].difficulty) != EOF) {
        i++;
    }

    fclose(file);
}

void shuffleQuestions(Question questions[], int count) {
    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        int j = rand() % count;
        Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

void askQuestion(Question *q, int *score) {
    printf("\n\033[1;34m%s\033[0m\n", q->question); // Blue for question

    for (int i = 0; i < 4; i++) {
        printf("%d. %s\n", i + 1, q->options[i]);
    }

    int answer;
    char input[10];

    while (1) {
        printf("Choose an answer (1-4): ");
        scanf("%s", input);

        // Check if input is numeric
        if (strlen(input) == 1 && input[0] >= '1' && input[0] <= '4') {
            answer = input[0] - '0'; // convert char to int
            break;
        } else {
            printf("Invalid answer. Please choose a number between 1 and 4.\n");
        }
    }
    if (answer == q->correctAnswer) {
        *score += q->difficulty;
        printf("\033[0;32mCorrect!\033[0m\n"); // Green
    } else {
        printf("\033[0;31mWrong!\033[0m\n"); // Red
    }
}


void saveGame(const char *playerName, int questionIndex, int score) {
    FILE *file = fopen(SAVED_GAME_FILE, "w");
    if (file == NULL) {
        printf("\nError saving game.\n");
        return;
    }

    fprintf(file, "%s %d %d\n", playerName, questionIndex, score);
    fclose(file);
}

int loadSavedGame(char *playerName, int *questionIndex, int *score) {
    FILE *file = fopen(SAVED_GAME_FILE, "r");
    if (file == NULL) {
        return 0;
    }

    fscanf(file, "%s %d %d", playerName, questionIndex, score);
    fclose(file);
    return 1;
}

void startGame() {
    Question questions[TOTAL_QUESTIONS];
    loadQuestions(questions);
    shuffleQuestions(questions, TOTAL_QUESTIONS);

    char playerName[NAME_LENGTH];
    int score = 0;

    printf("\nEnter your name: ");
    scanf("%s", playerName);

    for (int i = 0; i < GAME_QUESTIONS; i++) {
        askQuestion(&questions[i], &score);
        saveGame(playerName, i + 1, score);
    }

    printf("\nGame Over! Your final score: %d\n", score);
    saveScore(playerName, score);
}

void addQuestion() {
    FILE *file = fopen(QUESTIONS_FILE, "a");
    if (file == NULL) {
        printf("\nError opening questions file for appending.\n");
        return;
    }

    Question q;
    printf("\nEnter the question: ");
    getchar();
    fgets(q.question, sizeof(q.question), stdin);
    q.question[strcspn(q.question, "\n")] = '\0';

    for (int i = 0; i < 4; i++) {
        printf("Enter option %d: ", i + 1);
        fgets(q.options[i], sizeof(q.options[i]), stdin);
        q.options[i][strcspn(q.options[i], "\n")] = '\0';
    }

    printf("Enter correct answer (1-4): ");
    scanf("%d", &q.correctAnswer);
    printf("Enter difficulty (1=easy, 2=medium, 3=hard): ");
    scanf("%d", &q.difficulty);

    fprintf(file, "%s|%s|%s|%s|%s|%d|%d\n",
            q.question,
            q.options[0],
            q.options[1],
            q.options[2],
            q.options[3],
            q.correctAnswer,
            q.difficulty);

    fclose(file);
    printf("Question added successfully!\n");
}

void resumeGame() {
    char playerName[NAME_LENGTH];
    int score = 0, questionIndex = 0;

    if (!loadSavedGame(playerName, &questionIndex, &score)) {
        printf("\nNo saved game found. Starting a new game instead.\n");
        startGame();
        return;
    }

    Question questions[TOTAL_QUESTIONS];
    loadQuestions(questions);
    shuffleQuestions(questions, TOTAL_QUESTIONS);

    printf("\nResuming game for %s\n", playerName);
    printf("Current score: %d\n", score);

    for (int i = questionIndex; i < GAME_QUESTIONS; i++) {
        askQuestion(&questions[i], &score);
        saveGame(playerName, i + 1, score);
    }

    printf("\nGame Over! Your final score: %d\n", score);
    saveScore(playerName, score);
}

void showLeaderboard() {
    system("cls");

    FILE *file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        printf("\nNo leaderboard found.\n");
        return;
    }

    char name[NAME_LENGTH];
    int score;

    printf("\n\033[1;33mLeaderboard:\033[0m\n"); // Yellow
    while (fscanf(file, "%s %d", name, &score) != EOF) {
        printf("\033[1;36m%s\033[0m: \033[0;32m%d\033[0m\n", name, score); // Cyan name, green score
    }

    fclose(file);
    system("pause");
    system("cls");
}


void saveScore(const char *name, int score) {
    FILE *file = fopen(LEADERBOARD_FILE, "a");
    if (file == NULL) {
        printf("\nError saving score.\n");
        return;
    }

    fprintf(file, "%s %d\n", name, score);
    fclose(file);
}

int main() {
    int choice;

    while (1) {
        showMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                startGame();
                break;
            case 2:
                resumeGame();
                break;
            case 3:
                showLeaderboard();
                break;
            case 4:
                addQuestion();
                break;
            case 5:
                printf("\nExiting the game...\n");
                return 0;
            default:
                printf("\nInvalid choice, try again.\n");
        }
    }

    return 0;
}
