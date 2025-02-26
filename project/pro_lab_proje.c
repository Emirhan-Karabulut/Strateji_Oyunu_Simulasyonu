#define NOUSER //Closewindow çakışması için
#define WIN32_LEAN_AND_MEAN //LPMSG çakışması için
#define NOGDI //Rectangle çakışması için
#define GRID_SIZE 20

#include <curl/curl.h>
#include <windows.h>
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef struct {
    char creature_medeniyet[30];
    char creature_ad[30];
    int creature_etki_degeri;
    char creature_etki_turu[30];
    char creature_aciklama[150];
    char creature_etkilenen_birim[30];
}Creature;

typedef struct {
    char heroes_medeniyet[30];
    char heroes_ad[30];
    char heroes_bonus_turu[30];
    int heroes_bonus_degeri; 
    char heroes_aciklama[150];
    char heroes_etkilenen_birim[30];
}Heroes;

typedef struct {
    char research_ad[30];
    char research_seviye[30];
    int research_deger;
    char research_turu[30];
    char research_aciklama[150];
    char etkilenen_birim[30];
}Research;

typedef struct {
    char unit_medeniyet[30];
    char unit_ad[30];
    int unit_saldiri;
    int unit_savunma;
    int unit_saglik;
    int unit_kritik_sans;
}unit_types;

typedef struct {
    char medeniyet[50];            
    char kahramanlar[10][50];
    int kahraman_sayisi;
    char canavarlar[10][50];
    int canavar_sayisi;
    char arastirma_turu[10][50];
    int arastirma_seviyesi[10];  //3 yazar
    int arastirma_sayisi;
    char arastirma_seviye_deger[10][30];  //seviye_3 yazar
    char birim_adlari[5][50];         
    int birim_adetleri[5];             
    int toplam_birim_adedi;          
} Scenario;

typedef struct {
    char medeniyet[50];
    char birim_ad[10][50];
    int birim_sayilari[10];
    int birim_canlari[10];
    int birim_max_canlari[10];
    int birim_savunmalari[10];
    int birim_saldirilari[10];
    float birim_kritik_yuzdeleri[10];
    int toplam_can;
    int toplam_savunma;
    int toplam_saldiri;
    int birim_koordinat_x[10];
    int birim_koordinat_y[10];
    char kahramanlar[10][50];
    char kahraman_aciklama[10][50];
    int kahraman_sayisi;
    char canavarlar[10][50];
    char canavar_aciklama[10][50];
    int canavar_sayisi;
    char arastirma_ad[10][100];
    char arastirma_seviye[30];
    char arastirma_aciklama[10][100];
    char kahraman_buff_birimleri[10][50];
    char canavar_buff_birimleri[10][50];

} simulasyon;

typedef struct {
    simulasyon* sim;
    int birim_index;
    int count;
    int hero_count;
    int creature_count;
    char type[15]; // Hücre tipi: 'U' = Birim, 'H' = Kahraman, 'C' = Canavar
} Cell;

typedef struct {
    char medeniyet[50];
    char birim_ad[10][50];
    int birim_sayilari[10];
    int birim_canlari[10];
    int birim_savunmalari[10];
    int birim_saldirilari[10];
    int birim_kritik_yuzdeleri[10];
} Firstvalue;

Cell grid[GRID_SIZE][GRID_SIZE] = {0};
int CELL_SIZE;
Texture2D grassTexture;
Texture2D unitTextures[2][10];
Texture2D kahramanTextures[2][10];
Texture2D canavarTextures[2][10];
Texture2D backgroundTexture;
Rectangle backgroundRect;
Rectangle fullScreenRect;

void print_scenario(simulasyon *s, FILE *file, Firstvalue *f);
size_t write_to_file(void *data, size_t size, size_t nmemb, FILE *file);
bool is_file_empty(const char *filepath);
void download_and_save_json(const char *url, const char *dosya_yolu);
void extract_creature_unit_from_description(Creature *creature);
void extract_hero_unit_from_description(Heroes *heroes);
void read_creatures(const char *filename, Creature creatures[], int max_creatures);
void read_heroes(const char *filename, Heroes heroes[], int max_heroes);
void read_research(const char *filename, Research research[], int max_research);
void read_units(const char *filename, unit_types units[], int max_units);
void empty_scenario(Scenario *s);
void read_scenarios(const char *filename, Scenario scenarios[]);
void LoadAllTextures(simulasyon senaryo[]);
void UnloadAllTextures();
Color GetHealthBarColor(float healthPercentage);
void drawCell(int posX, int posY, Cell cell,int round,simulasyon *senaryo,int sayac, int sayac2);
void DrawBackground();
void DrawFullBackground();
void DrawCenteredGrid(int round,simulasyon *senaryo,int sayac, int sayac2);
void AdjustGridSize();
void placeUnits(simulasyon senaryo[]);
void resetCell(Cell *cell);
void resetGrid();
void InitializeGame(simulasyon* newsenaryo);
void DrawNextButton();
void DrawSummaryButton();
bool IsNextButtonClicked();
bool IsSummaryButtonClicked();
void calculate(Scenario* scenarios, unit_types* units, Creature* creatures, Heroes* heroes, Research* researches, simulasyon* senaryo,char medeniyetler[2][50],FILE *file, int birim_sayisi);
void simulate(simulasyon* senaryo,char medeniyetler[2][50], FILE* file,int a,int b, int* current_state,int *round,int *attacker, int *defender,int *sayac,int *sayac2);

int main() {
    int sayi;
    char url[100] = "https://yapbenzet.org.tr/";
    char dosya_yolu[100] = "C:/project/senaryo.json";
    FILE *file = fopen("savas_sim.txt", "w");
    char medeniyetler[2][50] = {"insan_imparatorlugu", "ork_legi"};

    Creature creatures[50];
    Heroes heroes[50];
    Research research[50];
    unit_types units[50];
    Scenario scenarios[10];
    simulasyon senaryo[2];

    InitWindow(1620, 1000, "Battle Visualization");
    ToggleFullscreen();
    SetTargetFPS(60);

    int attacker = 0;
    int defender = 1;
    int round = 1;
    int current_state = 1;
    int a=0;
    int b=0;
    int total_units_0 = 0;
    int total_units_1 = 0;
    int sayac = 1;
    int sayac2= 1;
    resetGrid();
    InitializeGame(senaryo);

    while (!WindowShouldClose()) {
        BeginDrawing();
        total_units_0 = 0;
        total_units_1 = 0;
        for(int i = 0; i < 10; i++) {
            if(senaryo[0].birim_sayilari[i] > 0) {
                total_units_0 += senaryo[0].birim_sayilari[i];
            }
        }

        for(int i = 0; i < 10; i++) {
            if(senaryo[1].birim_sayilari[i] > 0) {
                total_units_1 += senaryo[1].birim_sayilari[i];
            }
        }

        switch (current_state) {
            case 1: {

                DrawFullBackground();
                const char* title = "Senaryo Secimi";
                int fontSize = 40;
                float titleWidth = MeasureText(title, fontSize);
                DrawText(title,(GetScreenWidth() - titleWidth) / 2,GetScreenHeight() / 4,fontSize,BLACK);

                const int buttonWidth = 100;
                const int buttonHeight = 50;
                const int spacing = 20;
                const int buttonsPerRow = 5;
                float totalWidth = (buttonsPerRow * buttonWidth) + ((buttonsPerRow - 1) * spacing);
                float startX = (GetScreenWidth() - totalWidth) / 2;
                float startY = GetScreenHeight() / 2 - buttonHeight;
                Vector2 mousePoint = GetMousePosition();

                for (int i = 0; i < 10; i++) {
                    int row = i / buttonsPerRow;
                    int col = i % buttonsPerRow;
                    float buttonX = startX + (col * (buttonWidth + spacing));
                    float buttonY = startY + (row * (buttonHeight + spacing));

                    Rectangle buttonBounds = {buttonX,buttonY,buttonWidth,buttonHeight};

                    bool isHovered = CheckCollisionPointRec(mousePoint, buttonBounds);
                    if (isHovered)
                        DrawRectangleRec(buttonBounds, DARKGRAY);
                    else
                        DrawRectangleRec(buttonBounds, GRAY);

                    char numberStr[3];
                    sprintf(numberStr, "%d", i + 1);
                    float numberWidth = MeasureText(numberStr, 20);
                    DrawText(numberStr,buttonX + (buttonWidth - numberWidth) / 2,buttonY + (buttonHeight - 20) / 2,20,WHITE);

                    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        sayi = i + 1;
                        // Reset URL to base before adding new scenario number
                        strcpy(url, "https://yapbenzet.org.tr/");
                        sprintf(url + strlen(url), "%d.json", sayi);

                        download_and_save_json(url, dosya_yolu);
                        read_creatures("C:/project/Files/creatures.json", creatures, 50);
                        read_heroes("C:/project/Files/heroes.json", heroes, 50);
                        read_research("C:/project/Files/research.json", research, 50);
                        read_units("C:/project/Files/unit_types.json", units, 50);
                        read_scenarios(dosya_yolu, scenarios);
                        calculate(scenarios, units, creatures, heroes, research, senaryo, medeniyetler, file, 50);

                        resetGrid();
                        InitializeGame(senaryo);
                        AdjustGridSize();

                        current_state = 2;
                        break;
                    }
                }
                break;
            }
            case 2: {
                AdjustGridSize();
                DrawCenteredGrid(round, senaryo,sayac,sayac2);
                DrawNextButton();
                DrawSummaryButton();

                if (IsKeyPressed(KEY_ENTER) || IsNextButtonClicked()) {
                    current_state = 3;
                }
                else if (IsKeyPressed(KEY_SPACE) || IsSummaryButtonClicked()) {
                    a=1;
                    b=1;
                    current_state = 4;
                }
                break;
            }

            case 3: {
                if(total_units_0 !=0 && total_units_1 != 0) {
                    simulate(senaryo, medeniyetler, file, a,b, &current_state, &round,&attacker,&defender,&sayac,&sayac2);
                    resetGrid();
                    InitializeGame(senaryo);AdjustGridSize();

                }

                DrawCenteredGrid(round,senaryo,sayac,sayac2);
                break;
            }

            case 4: {
                if(total_units_0 !=0 && total_units_1 != 0) {
                    simulate(senaryo, medeniyetler, file, a,b, &current_state, &round,&attacker,&defender,&sayac,&sayac2);
                    resetGrid();
                    InitializeGame(senaryo);
                    AdjustGridSize();

                }
                DrawCenteredGrid(round,senaryo,sayac,sayac2);
                break;
            }
        }
        EndDrawing();
    }
    fclose(file);
    UnloadAllTextures();
    CloseWindow();
    return 0;
}

// function to write initial data to the file
void print_scenario(simulasyon *s, FILE *file, Firstvalue *f) {
    fprintf(file, "Medeniyet: %s\n", s->medeniyet);

    fprintf(file, "Birimler:\n");
    for (int i = 0; i < 10; i++) {
        if (s->birim_sayilari[i] > 0) {
            fprintf(file, "  %d. %s: %d birim (Sağlık: %d/%d, Saldırı: %d, Savunma: %d, Kritik: %d%%)\n",i + 1, f->birim_ad[i], f->birim_sayilari[i],f->birim_canlari[i], f->birim_canlari[i],f->birim_saldirilari[i], f->birim_savunmalari[i],f->birim_kritik_yuzdeleri[i]);
        }
    }

    fprintf(file, "Kahramanlar:\n");
    for (int i = 0; i < s->kahraman_sayisi; i++) {
        fprintf(file, "  %d. %s (%s)\n", i + 1, s->kahramanlar[i], s->kahraman_aciklama[i]);
    }

    fprintf(file, "Canavarlar:\n");
    for (int i = 0; i < s->canavar_sayisi; i++) {
        fprintf(file, "  %d. %s (%s)\n", i + 1, s->canavarlar[i], s->canavar_aciklama);
    }
}

//Writes incoming data directly to the file.
size_t write_to_file(void *data, size_t size, size_t nmemb, FILE *file) {
    return fwrite(data, size, nmemb, file);
}

//Function that gives an error if data cannot be retrieved from the server using the curl library
bool is_file_empty(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return true;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size <= 0;
}

//Function to retrieve data from the server and save it in a json file using the curl library
void download_and_save_json(const char *url, const char *dosya_yolu) {
    CURL *curl;
    CURLcode res;
    FILE *file;
    bool download_success = false;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        file = fopen(dosya_yolu, "wb");
        if (file == NULL) {
            fprintf(stderr, "Hata: Dosya acilamadi!\n");
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        res = curl_easy_perform(curl);
        fclose(file);

        if (res != CURLE_OK) {
            fprintf(stderr, "Hata: Internet baglantisi veya veri Indirme hatasi: %s\n", curl_easy_strerror(res));
            remove(dosya_yolu);
        } else {
            if (is_file_empty(dosya_yolu)) {
                fprintf(stderr, "Hata: Indirilen veri bos veya gecersiz!\n");
                remove(dosya_yolu);
            } else {
                printf("Veriler basariyla %s dosyasina yazildi.\n", dosya_yolu);
                download_success = true;
            }
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    if (!download_success) {
        exit(1);
    }
}

// Function to extract unit from description and store in creature_etkilenen_birim
void extract_creature_unit_from_description(Creature *creature) {
    strcpy(creature->creature_etkilenen_birim, "belirtilmedi");

    if (strstr(creature->creature_aciklama, "piyade")) {
        strcpy(creature->creature_etkilenen_birim, "piyadeler");
    }
    if (strstr(creature->creature_aciklama, "suvari")) {
        strcpy(creature->creature_etkilenen_birim, "suvariler");
    }
    if (strstr(creature->creature_aciklama, "ork_dovuscu")) {
        strcpy(creature->creature_etkilenen_birim, "ork_dovusculeri");
    }
    if (strstr(creature->creature_aciklama, "okcu")) {
        strcpy(creature->creature_etkilenen_birim, "okcular");
    }
    if (strstr(creature->creature_aciklama, "trol")) {
        strcpy(creature->creature_etkilenen_birim, "troller");
    }
    if (strstr(creature->creature_aciklama, "varg_binici")) {
        strcpy(creature->creature_etkilenen_birim, "varg_binicileri");
    }
    if (strstr(creature->creature_aciklama, "mizrakci")) {
        strcpy(creature->creature_etkilenen_birim, "mizrakcilar");
    }
}

// Function to extract unit from description and store in heroes_etkilenen_birim
void extract_hero_unit_from_description(Heroes *heroes) {
    strcpy(heroes->heroes_etkilenen_birim, "belirtilmedi");

    if (strstr(heroes->heroes_aciklama, "piyade")) {
        strcpy(heroes->heroes_etkilenen_birim, "piyadeler");
    }
    if (strstr(heroes->heroes_aciklama, "kusatma_makinelerinin")) {
        strcpy(heroes->heroes_etkilenen_birim, "kusatma_makineleri");
    }
    if (strstr(heroes->heroes_aciklama, "okcu")) {
        strcpy(heroes->heroes_etkilenen_birim, "okcular");
    }
    if (strstr(heroes->heroes_aciklama, "suvari")) {
        strcpy(heroes->heroes_etkilenen_birim, "suvariler");
    }
    if (strstr(heroes->heroes_aciklama, "ork_dovuscu")) {
        strcpy(heroes->heroes_etkilenen_birim, "ork_dovusculeri");
    }
    if (strstr(heroes->heroes_aciklama, "trol")) {
        strcpy(heroes->heroes_etkilenen_birim, "troller");
    }
    if (strstr(heroes->heroes_aciklama, "varg_binici")) {
        strcpy(heroes->heroes_etkilenen_birim, "varg_binicileri");
    }
    if (strstr(heroes->heroes_aciklama, "tum_birimlere")) {
        strcpy(heroes->heroes_etkilenen_birim, "tum_birimler");
    }
}

// Function to read and process creatures from the file
void read_creatures(const char *filename, Creature creatures[], int max_creatures) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dosya acilamadi!\n");
        exit(1);
    }

    int index = 0;
    char buffer[1000];
    char current_medeniyet[30];

    while (fgets(buffer, sizeof(buffer), file) && index < max_creatures) {
        if (strstr(buffer, "insan_imparatorlugu") || strstr(buffer, "ork_legi")) {
            sscanf(buffer, " \"%[^\"]", current_medeniyet);
            /*
                " \"%[^\"]" this manual expression takes the text between two quotes. It is a feature of the C language
                (first it skips the space at the beginning of the line ( ) then it skips the first quote (") and then saves all expressions until it sees the second quote ("))
            */
        }

        if (strstr(buffer, "\"") && strstr(buffer, ": {")) {
            sscanf(buffer, " \"%[^\"]", creatures[index].creature_ad);
            strcpy(creatures[index].creature_medeniyet, current_medeniyet);
        }

        if (strstr(buffer, "etki_degeri")) {
            sscanf(buffer, " \"etki_degeri\": \"%d\"", &creatures[index].creature_etki_degeri);
        }

        if (strstr(buffer, "etki_turu")) {
            sscanf(buffer, " \"etki_turu\": \"%[^\"]", creatures[index].creature_etki_turu);
        }

        if (strstr(buffer, "aciklama")) {
            sscanf(buffer, " \"aciklama\": \"%[^\"]", creatures[index].creature_aciklama);

            extract_creature_unit_from_description(&creatures[index]);
            index++;
        }
    }

    fclose(file);
}

// Function to read and process heroes from the file
void read_heroes(const char *filename, Heroes heroes[], int max_heroes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dosya acilamadi!\n");
        exit(1);
    }

    int index = 0;
    char buffer[512];
    char current_medeniyet[30];

    while (fgets(buffer, sizeof(buffer), file) && index < max_heroes) {
        if (strstr(buffer, "insan_imparatorlugu") || strstr(buffer, "ork_legi")) {
            sscanf(buffer, " \"%[^\"]", current_medeniyet);
        }

        if (strstr(buffer, "\"") && strstr(buffer, ": {")) {
            sscanf(buffer, " \"%[^\"]", heroes[index].heroes_ad);
            strcpy(heroes[index].heroes_medeniyet, current_medeniyet);
        }

        if (strstr(buffer, "bonus_turu")) {
            sscanf(buffer, " \"bonus_turu\": \"%[^\"]", heroes[index].heroes_bonus_turu);
        }

        if (strstr(buffer, "bonus_degeri")) {
            sscanf(buffer, " \"bonus_degeri\": \"%d\"", &heroes[index].heroes_bonus_degeri);
        }

        if (strstr(buffer, "aciklama")) {
            sscanf(buffer, " \"aciklama\": \"%[^\"]", heroes[index].heroes_aciklama);
            extract_hero_unit_from_description(&heroes[index]);
            index++;
        }
    }

    fclose(file);
}

// Function to read and process research from the file
void read_research(const char *filename, Research research[], int max_research) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dosya acilamadi!\n");
        exit(1);
    }

    int index = 0;
    char buffer[512];
    char current_research_ad[30];
    while (fgets(buffer, sizeof(buffer), file) && index < max_research) {
        if (strstr(buffer, "savunma_ustaligi") || strstr(buffer, "saldiri_gelistirmesi") ||
            strstr(buffer, "elit_egitim") || strstr(buffer, "kusatma_ustaligi")) {
            sscanf(buffer, " \"%[^\"]", current_research_ad);
        }

        if (strstr(buffer, "\"seviye_")) {
            sscanf(buffer, " \"%[^\"]", research[index].research_seviye);
        }

        if (strstr(buffer, "deger")) {
            sscanf(buffer, " \"deger\": \"%d\"", &research[index].research_deger);
        }

        if (strstr(buffer, "aciklama")) {
            sscanf(buffer, " \"aciklama\": \"%[^\"]", research[index].research_aciklama);
            if (strstr(research[index].research_aciklama, "saldiri") ||
                strstr(research[index].research_aciklama, "savunma") ||
                strstr(research[index].research_aciklama, "kritik")) {

                strcpy(research[index].research_ad, current_research_ad);

                if (strstr(research[index].research_aciklama, "saldiri")) {
                    strcpy(research[index].research_turu, "saldiri");
                    strcpy(research[index].etkilenen_birim, "tum_birimler");
                } else if (strstr(research[index].research_aciklama, "savunma")) {
                    strcpy(research[index].research_turu, "savunma");
                    strcpy(research[index].etkilenen_birim, "tum_birimler");
                } else if (strstr(research[index].research_aciklama, "kritik")) {
                    strcpy(research[index].research_turu, "kritik");
                    strcpy(research[index].etkilenen_birim, "tum_birimler");
                }

                if (strstr(current_research_ad, "kusatma_ustaligi")) {
                    strcpy(research[index].etkilenen_birim, "kusatma_makineleri");
                } else if (strstr(current_research_ad, "elit_egitim")) {
                    strcpy(research[index].etkilenen_birim, "tum_birimler");
                }

                index++;
            }
        }
    }
    fclose(file);
}

// Function to read and process units from the file
void read_units(const char *filename, unit_types units[], int max_units) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dosya acilamadi!\n");
        exit(1);
    }

    int index = 0;
    char buffer[512];
    char current_medeniyet[30];

    while (fgets(buffer, sizeof(buffer), file) && index < max_units) {
        if (strstr(buffer, "insan_imparatorlugu") || strstr(buffer, "ork_legi")) {
            sscanf(buffer, " \"%[^\"]", current_medeniyet);
        }

        if (strstr(buffer, "\"") && strstr(buffer, ": {")) {
            sscanf(buffer, " \"%[^\"]", units[index].unit_ad);
            strcpy(units[index].unit_medeniyet, current_medeniyet);
        }

        if (strstr(buffer, "saldiri")) {
            sscanf(buffer, " \"saldiri\": %d", &units[index].unit_saldiri);
        }

        if (strstr(buffer, "savunma")) {
            sscanf(buffer, " \"savunma\": %d", &units[index].unit_savunma);
        }

        if (strstr(buffer, "saglik")) {
            sscanf(buffer, " \"saglik\": %d", &units[index].unit_saglik);
        }

        if (strstr(buffer, "kritik_sans")) {
            sscanf(buffer, " \"kritik_sans\": %d", &units[index].unit_kritik_sans);
            index++;
        }
    }

    fclose(file);
}

//Function to set every value to zero at start
void empty_scenario(Scenario *s) {
    memset(s, 0, sizeof(Scenario));
}

//Function to read every value in scenario json
void read_scenarios(const char *filename, Scenario scenarios[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dosya acilamadi!\n");
        exit(1);
    }

    int index = -1;
    char buffer[512];

    while (fgets(buffer, sizeof(buffer), file)) {
        // Read civilization name (medeniyet)
        if (strstr(buffer, "insan_imparatorlugu") || strstr(buffer, "ork_legi")) {
            index++;
            memset(&scenarios[index], 0, sizeof(Scenario));
            sscanf(buffer, " \"%[^\"]", scenarios[index].medeniyet);
        }

        if (strstr(buffer, "kahramanlar")) {
            char heroes_buffer[150];
            memset(heroes_buffer, 0, sizeof(heroes_buffer));
            char *token = strtok(buffer, "[");
            token = strtok(NULL, "]");
            if (token != NULL) {
                token = strtok(token, ",");
                int hero_index = 0;
                while (token && hero_index < 10) {
                    char *newline_pos = strchr(token, '\n');
                    if (newline_pos) {
                        *newline_pos = '\0';
                    }

                    char *start_quote = strchr(token, '\"');
                    char *end_quote = strrchr(token, '\"');
                    if (start_quote) {
                        start_quote++;
                    }
                    if (end_quote) {
                        *end_quote = '\0';
                    }

                    if (start_quote)
                        strcpy(scenarios[index].kahramanlar[hero_index], start_quote);
                    else
                        strcpy(scenarios[index].kahramanlar[hero_index], token);

                    hero_index++;
                    token = strtok(NULL, ",");
                }
                scenarios[index].kahraman_sayisi = hero_index;
            }
        }

        if (strstr(buffer, "canavarlar")) {
            char monsters_buffer[150];
            memset(monsters_buffer, 0, sizeof(monsters_buffer));
            char *token = strtok(buffer, "[");
            token = strtok(NULL, "]");
            if (token != NULL) {
                token = strtok(token, ",");
                int monster_index = 0;
                while (token && monster_index < 10) {
                    char *newline_pos = strchr(token, '\n');
                    if (newline_pos) {
                        *newline_pos = '\0';
                    }

                    char *start_quote = strchr(token, '\"');
                    char *end_quote = strrchr(token, '\"');
                    if (start_quote) {
                        start_quote++;
                    }
                    if (end_quote) {
                        *end_quote = '\0';
                    }

                    if (start_quote)
                        strcpy(scenarios[index].canavarlar[monster_index], start_quote);
                    else
                        strcpy(scenarios[index].canavarlar[monster_index], token);

                    monster_index++;
                    token = strtok(NULL, ",");
                }
                scenarios[index].canavar_sayisi = monster_index;
            }
        }

        if (strstr(buffer, "arastirma_seviyesi")) {
            scenarios[index].arastirma_sayisi = 0;
            while (fgets(buffer, sizeof(buffer), file) && strstr(buffer, "}") == NULL) {
                char research_type[50];
                int research_level;
                if (sscanf(buffer, " \"%[^\"]\": %d,", research_type, &research_level) == 2) {
                    strcpy(scenarios[index].arastirma_turu[scenarios[index].arastirma_sayisi], research_type);
                    scenarios[index].arastirma_seviyesi[scenarios[index].arastirma_sayisi] = research_level;

                    snprintf(scenarios[index].arastirma_seviye_deger[scenarios[index].arastirma_sayisi],
                            sizeof(scenarios[index].arastirma_seviye_deger[scenarios[index].arastirma_sayisi]),
                            "seviye_%d", research_level);

                    scenarios[index].arastirma_sayisi++;
                }
            }
        }

        if (strstr(buffer, "birimler")) {
            int unit_index = 0;
            char unit_name[50];
            int unit_count;

            while (fgets(buffer, sizeof(buffer), file) && strstr(buffer, "}") == NULL) {
                sscanf(buffer, " \"%[^\"]\": %d,", unit_name, &unit_count);
                strcpy(scenarios[index].birim_adlari[unit_index], unit_name);
                scenarios[index].birim_adetleri[unit_index] = unit_count;
                scenarios[index].toplam_birim_adedi += unit_count;
                unit_index++;
            }
        }
    }
    fclose(file);
}

//Function to load all textures
void LoadAllTextures(simulasyon senaryo[]) {

    backgroundTexture = LoadTexture("C:/project/Textures/background.png");
    fullScreenRect = (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() };
    grassTexture = LoadTexture("C:/project/Textures/cimen.png");
    int totalGridWidth = GRID_SIZE * CELL_SIZE;
    int totalGridHeight = GRID_SIZE * CELL_SIZE;
    int startX = (GetScreenWidth() - totalGridWidth) / 2;
    int startY = (GetScreenHeight() - totalGridHeight) / 2;
    backgroundRect = (Rectangle){ startX, startY, totalGridWidth, totalGridHeight };

    for (int s = 0; s < 2; s++) {
        simulasyon *sim = &senaryo[s];

        for (int i = 0; i < 10; i++) {
            if (sim->birim_ad[i][0] != '\0') {
                char imagePath[256];
                snprintf(imagePath, sizeof(imagePath),
                         "C:/project/Textures/Units/%s_%s.png",
                         sim->medeniyet, sim->birim_ad[i]);
                unitTextures[s][i] = LoadTexture(imagePath);
            }
        }

        for (int i = 0; i < sim->kahraman_sayisi; i++) {
            if (sim->kahramanlar[i][0] != '\0') {
                char imagePath[256];
                snprintf(imagePath, sizeof(imagePath),
                         "C:/project/Textures/Heroes/%s_%s.png",
                         sim->medeniyet, sim->kahramanlar[i]);
                kahramanTextures[s][i] = LoadTexture(imagePath);
            }
        }

        for (int i = 0; i < sim->canavar_sayisi; i++) {
            if (sim->canavarlar[i][0] != '\0') {
                char imagePath[256];
                snprintf(imagePath, sizeof(imagePath),
                         "C:/project/Textures/Creatures/%s_%s.png",
                         sim->medeniyet, sim->canavarlar[i]);
                canavarTextures[s][i] = LoadTexture(imagePath);
            }
        }
    }
}

//Function to unload all textures
void UnloadAllTextures() {
    for (int s = 0; s < 2; s++) {
        for (int i = 0; i < 10; i++) {
            UnloadTexture(unitTextures[s][i]);
            UnloadTexture(kahramanTextures[s][i]);
            UnloadTexture(canavarTextures[s][i]);
        }
    }
    UnloadTexture(backgroundTexture);
    UnloadTexture(grassTexture);
}

//Function to set the color of the health bar
Color GetHealthBarColor(float normalizedHealthPercentage)
{
    if (normalizedHealthPercentage >= 80) return GREEN;
    if (normalizedHealthPercentage >= 20) return YELLOW;
    return RED;
}

//Function for visualization
void drawCell(int posX, int posY, Cell cell, int round,simulasyon *senaryo,int sayac, int sayac2) {
    DrawRectangleLines(posX, posY, CELL_SIZE, CELL_SIZE, Fade(LIGHTGRAY, 0.5f));

    char roundText[32];
    snprintf(roundText, sizeof(roundText), "Round: %d", (int)ceil((float)round/2));
    int roundFontSize = 30;
    int roundTextWidth = MeasureText(roundText, roundFontSize);
    DrawRectangle(GetScreenWidth() - roundTextWidth - 30, 50, roundTextWidth + 20, 40, Fade(BLACK, 0.7f));
    DrawText(roundText, GetScreenWidth() - roundTextWidth - 20, 55, roundFontSize, WHITE);

    bool unitFound = false;
    bool drawRedCross = false;

    if (posX < GetScreenWidth() / 2) {
        DrawRectangle(70, 10, 230, 30, Fade(BLACK, 0.7f));
        DrawText("Insan_Imparatorlugu", 80, 20, 20, WHITE);
    } else {
        int textWidth = MeasureText("Ork_Legi", 20);
        DrawRectangle(GetScreenWidth() - textWidth - 200, 10, textWidth + 20, 30, Fade(BLACK, 0.7f));
        DrawText("Ork_Legi", GetScreenWidth() - textWidth - 190, 20, 20, WHITE);
    }

    if (cell.sim) {
        Texture2D unitTexture;
        const char *unitName;
        int unitCount = cell.count;
        int simIndex = cell.sim - senaryo;

        int totalUnitsHuman = 0;
        int totalUnitsOrc = 0;
        for (int i = 0; i < 10; i++) {
            totalUnitsHuman += senaryo[0].birim_sayilari[i];
            totalUnitsOrc += senaryo[1].birim_sayilari[i];
        }

        if (totalUnitsHuman == 0 || totalUnitsOrc == 0) {
            const char* winner;
            if (totalUnitsHuman == 0) {
                winner = "Ork_Legi Kazandi!";
            } else {
                winner = "Insan_Imparatorlugu Kazandi!";
            }

            int fontSize = 40;
            int textWidth = MeasureText(winner, fontSize);
            int textX = (GetScreenWidth() - textWidth) / 2;
            int textY = GetScreenHeight() / 2;

            DrawRectangle(textX - 10, textY - 10, textWidth + 20, fontSize + 20, Fade(BLACK, 0.8f));
            DrawText(winner, textX, textY, fontSize, YELLOW);
        }

        if (cell.type[0] == 'U') {
            unitTexture = unitTextures[simIndex][cell.birim_index];
            unitName = cell.sim->birim_ad[cell.birim_index];
            unitFound = true;
        } else if (cell.type[0] == 'H') {
            unitTexture = kahramanTextures[simIndex][cell.birim_index];
            unitName = cell.sim->kahramanlar[cell.birim_index];
            if ((simIndex == 0 && totalUnitsHuman == 0) || (simIndex == 1 && totalUnitsOrc == 0)) {
                drawRedCross = true;
            }
            unitFound = true;
        } else if (cell.type[0] == 'C') {
            unitTexture = canavarTextures[simIndex][cell.birim_index];
            unitName = cell.sim->canavarlar[cell.birim_index];
            if ((simIndex == 0 && totalUnitsHuman == 0) || (simIndex == 1 && totalUnitsOrc == 0)) {
                drawRedCross = true;
            }
            unitFound = true;
        }

        if (unitFound) {
            float scale = (float)(CELL_SIZE) / fmaxf(unitTexture.width, unitTexture.height)+0.015;
            int scaledWidth = unitTexture.width * scale;
            int scaledHeight = unitTexture.height * scale;
            int drawX = posX + (CELL_SIZE - scaledWidth) / 2;
            int drawY = posY + (CELL_SIZE - scaledHeight) / 2;
            DrawTextureEx(unitTexture, (Vector2){drawX, drawY}, 0.0f, scale, WHITE);

            char unitInfo[64];
            snprintf(unitInfo, sizeof(unitInfo), "%s (%d)", unitName, unitCount);
            int fontSize = 10;
            int textWidth = MeasureText(unitInfo, fontSize);
            int textX = posX + (CELL_SIZE - textWidth) / 2;
            int textY = posY + CELL_SIZE - fontSize + 10;
            DrawRectangle(textX - 2, textY - 2, textWidth + 4, fontSize + 4, Fade(BLACK, 0.7f));
            DrawText(unitInfo, textX, textY, fontSize, WHITE);

            if ((int)ceil((float)round/2.0) % 5 == 1 && (int)ceil((float)round/2.0) != 1) {
                const char* yorgunlukText = "YORGUN";
                int yorgunlukFontSize = 12;
                DrawText(yorgunlukText,posX + CELL_SIZE - 17, posY + 3 , yorgunlukFontSize, WHITE);
            }

            if (cell.type[0] == 'U') {
                bool kritikVurus = false;
                if (simIndex == 0 && cell.sim->birim_kritik_yuzdeleri[cell.birim_index] != 0) {
                    if (sayac * ((int)(100.0 / (cell.sim->birim_kritik_yuzdeleri[cell.birim_index]))) * 2 - 1 == round) {
                        kritikVurus = true;
                    }
                }
                if (simIndex == 1 && cell.sim->birim_kritik_yuzdeleri[cell.birim_index] != 0) {
                    if (sayac2 * ((int)(100 / (cell.sim->birim_kritik_yuzdeleri[cell.birim_index]))) * 2 == round) {
                        kritikVurus = true;
                    }
                }

                if (kritikVurus) {
                    const char* kritikText = "KRITIK!";
                    int kritikFontSize = 12;
                    DrawText(kritikText, posX + CELL_SIZE - 17, posY +20, kritikFontSize, WHITE);
                }
            }

            if (cell.type[0] == 'U') {
                int currentHealth = cell.sim->birim_canlari[cell.birim_index];
                int maxHealth = cell.sim->birim_max_canlari[cell.birim_index];
                float healthPercentage = (float)currentHealth / maxHealth;
                int barWidth = CELL_SIZE - 10;
                int barHeight = 10;
                int currentBarWidth = (int)(healthPercentage * barWidth);
                float normalizedHealthPercentage = (float)currentHealth / (float)maxHealth * 100;
                Color healthBarColor = GetHealthBarColor(normalizedHealthPercentage);

                int barY = posY + 65;
                int barX = posX + 5;
                DrawRectangle(barX, barY, barWidth, barHeight, Fade(BLACK, 0.5f));
                DrawRectangle(barX, barY, currentBarWidth, barHeight, healthBarColor);
                DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);

                char healthText[32];
                snprintf(healthText, sizeof(healthText), "%d/%d", currentHealth, maxHealth);
                int healthFontSize = 8;
                int healthTextWidth = MeasureText(healthText, healthFontSize);
                int healthTextX = posX + (CELL_SIZE - healthTextWidth) / 2;
                int healthTextY = barY + barHeight + 2;
                DrawRectangle(healthTextX - 2, healthTextY - 2, healthTextWidth + 4, healthFontSize + 4, Fade(BLACK, 0.7f));
                DrawText(healthText, healthTextX, healthTextY, healthFontSize, WHITE);
            }
        }
    }

    if (drawRedCross) {
        for (int i = 0; i < 5; i++) {
            DrawLine(posX + i, posY, posX + CELL_SIZE + i, posY + CELL_SIZE, RED);
            DrawLine(posX + CELL_SIZE + i, posY, posX + i, posY + CELL_SIZE, RED);
        }
    }
}

//Function for visualization
void DrawBackground() {
    DrawTexturePro(grassTexture,(Rectangle){ 0, 0, grassTexture.width, grassTexture.height },backgroundRect,(Vector2){ 0, 0 },0.0f,WHITE);
}

//Function for visualization
void DrawFullBackground() {

    DrawTexturePro(backgroundTexture,(Rectangle){ 0, 0, backgroundTexture.width, backgroundTexture.height },fullScreenRect,(Vector2){ 0, 0 },0.0f,WHITE);
}

//Function for visualization
void DrawCenteredGrid(int round, simulasyon *senaryo,int sayac,int sayac2) {
    DrawFullBackground();
    DrawBackground();

    int totalGridWidth = GRID_SIZE * CELL_SIZE;
    int totalGridHeight = GRID_SIZE * CELL_SIZE;
    int startX = (GetScreenWidth() - totalGridWidth) / 2;
    int startY = (GetScreenHeight() - totalGridHeight) / 2;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int posX = startX + x * CELL_SIZE;
            int posY = startY + y * CELL_SIZE;
            drawCell(posX, posY, grid[y][x],round,senaryo,sayac,sayac2);
        }
    }
}

//Function for visualization
void AdjustGridSize() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int cellSizeWidth = screenWidth / GRID_SIZE;
    int cellSizeHeight = screenHeight / GRID_SIZE;

    if (cellSizeWidth < cellSizeHeight)
        CELL_SIZE = cellSizeWidth;
    else
        CELL_SIZE = cellSizeHeight;
}

//Function to place units into cells
void placeUnits(simulasyon senaryo[]) {
    for (int s = 0; s < 2; s++) {
        simulasyon *sim = &senaryo[s];
        if (strcmp(sim->medeniyet, "insan_imparatorlugu") == 0) {
            int x = 0, y = 5;

            for (int i = 0; i < sim->kahraman_sayisi; i++) {
                if (!grid[y][0].sim) {
                    grid[y][0].sim = sim;
                    grid[y][0].birim_index = i;
                    grid[y][0].count = 1;
                    grid[y][0].type[0] = 'H'; grid[y][0].type[1] = '\0';
                    sim->birim_koordinat_x[i] = 0;
                    sim->birim_koordinat_y[i] = y * 2;
                    y += 2;
                    if (y >= GRID_SIZE) break;
                }
            }

            for (int i = 0; i < sim->canavar_sayisi; i++) {
                if (!grid[y][0].sim) {
                    grid[y][0].sim = sim;
                    grid[y][0].birim_index = i;
                    grid[y][0].count = 1;
                    grid[y][0].type[0] = 'C'; grid[y][0].type[1] = '\0';
                    sim->birim_koordinat_x[i] = 0;
                    sim->birim_koordinat_y[i] = y * 2;
                    y += 2;
                    if (y >= GRID_SIZE) break;
                }
            }

            x = 2; y = 1;
            for (int i = 0; i < 10; i++) {
                int totalCount = sim->birim_sayilari[i];
                while (totalCount > 0) {
                    if (!grid[y][x].sim) {
                        grid[y][x].sim = sim;
                        grid[y][x].birim_index = i;
                        if (totalCount > 100)
                            grid[y][x].count = 100;
                        else
                            grid[y][x].count = totalCount;

                        grid[y][x].type[0] = 'U'; grid[y][x].type[1] = '\0';
                        totalCount -= grid[y][x].count;
                        sim->birim_koordinat_x[i] = x * 2;
                        sim->birim_koordinat_y[i] = y * 2;
                        y += 2;
                        if (y >= GRID_SIZE-1) {
                            y = 1;
                            x+=2;
                            if (x >= GRID_SIZE / 2) break;
                        }
                    }
                }
            }
        }
    }

    for (int s = 0; s < 2; s++) {
        simulasyon *sim = &senaryo[s];
        if (strcmp(sim->medeniyet, "ork_legi") == 0) {
            int x = GRID_SIZE - 1, y = 5;

            for (int i = 0; i < sim->kahraman_sayisi; i++) {
                if (!grid[y][GRID_SIZE - 1].sim) {
                    grid[y][GRID_SIZE - 1].sim = sim;
                    grid[y][GRID_SIZE - 1].birim_index = i;
                    grid[y][GRID_SIZE - 1].count = 1;
                    grid[y][GRID_SIZE - 1].type[0] = 'H'; grid[y][GRID_SIZE - 1].type[1] = '\0';
                    sim->birim_koordinat_x[i] = (GRID_SIZE - 1) * 2;
                    sim->birim_koordinat_y[i] = y * 2;
                    y += 2;
                    if (y >= GRID_SIZE) break;
                }
            }

            for (int i = 0; i < sim->canavar_sayisi; i++) {
                if (!grid[y][GRID_SIZE - 1].sim) {
                    grid[y][GRID_SIZE - 1].sim = sim;
                    grid[y][GRID_SIZE - 1].birim_index = i;
                    grid[y][GRID_SIZE - 1].count = 1;
                    grid[y][GRID_SIZE - 1].type[0] = 'C'; grid[y][GRID_SIZE - 1].type[1] = '\0';
                    sim->birim_koordinat_x[i] = (GRID_SIZE - 1) * 2;
                    sim->birim_koordinat_y[i] = y * 2;
                    y += 2;
                    if (y >= GRID_SIZE) break;
                }
            }

            x = GRID_SIZE - 3; y = 1;
            for (int i = 0; i < 10; i++) {
                int totalCount = sim->birim_sayilari[i];
                while (totalCount > 0) {
                    if (!grid[y][x].sim) {
                        grid[y][x].sim = sim;
                        grid[y][x].birim_index = i;
                        if (totalCount > 100)
                            grid[y][x].count = 100;
                        else
                            grid[y][x].count = totalCount;

                        grid[y][x].type[0] = 'U'; grid[y][x].type[1] = '\0';
                        totalCount -= grid[y][x].count;
                        sim->birim_koordinat_x[i] = x * 2;
                        sim->birim_koordinat_y[i] = y * 2;
                        y += 2;
                        if (y >= GRID_SIZE-1) {
                            y = 1;
                            x-=2;
                            if (x < GRID_SIZE / 2) break;
                        }
                    }
                }
            }
        }
    }
}

//Function to reset cell
void resetCell(Cell *cell) {
    memset(cell, 0, sizeof(*cell));
}

//Function to reset grid
void resetGrid() {
    memset(grid, 0, sizeof(grid));
}

//Function to start the game
void InitializeGame(simulasyon* newsenaryo) {

    AdjustGridSize();

    UnloadAllTextures();
    LoadAllTextures(newsenaryo);

    resetGrid();
    placeUnits(newsenaryo);
}

//Function to draw next button
void DrawNextButton()
{
    int buttonX = 1400;
    int buttonY = 100;
    int buttonWidth = 200;
    int buttonHeight = 50;
    Rectangle nextButtonRect = { (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight };

    if (CheckCollisionPointRec(GetMousePosition(), nextButtonRect))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {

        }
        else
        {
            DrawRectangleRec(nextButtonRect, Fade(GRAY, 0.5f));
        }
    }
    else
    {
        DrawRectangleRec(nextButtonRect, Fade(LIGHTGRAY, 0.5f));
    }

    DrawText("Sonraki", buttonX + 20, buttonY + 15, 20, BLACK);
}

//Function to put the game in automatic mode
void DrawSummaryButton()
{
    int buttonX = 1400;
    int buttonY = 900;
    int buttonWidth = 200;
    int buttonHeight = 50;
    Rectangle summaryButtonRect = { (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight };

    if (CheckCollisionPointRec(GetMousePosition(), summaryButtonRect))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {

        }
        else
        {
            DrawRectangleRec(summaryButtonRect, Fade(GRAY, 0.5f));
        }
    }
    else
    {
        DrawRectangleRec(summaryButtonRect, Fade(LIGHTGRAY, 0.5f));
    }

    DrawText("Otomatik Mod", buttonX + 20, buttonY + 15, 20, BLACK);
}

//Function to detect whether the next button is clicked or not
bool IsNextButtonClicked()
{
    int buttonX = 1400;
    int buttonY = 100;
    int buttonWidth = 200;
    int buttonHeight = 50;
    Rectangle nextButtonRect = { (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight };

    return CheckCollisionPointRec(GetMousePosition(), nextButtonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

//Function to detect whether the auto mode button is clicked or not
bool IsSummaryButtonClicked()
{
    int buttonX = 1400;
    int buttonY = 900;
    int buttonWidth = 200;
    int buttonHeight = 50;
    Rectangle summaryButtonRect = { (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight };

    return CheckCollisionPointRec(GetMousePosition(), summaryButtonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

//Function to make calculations and add bonuses
void calculate(Scenario* scenarios, unit_types* units, Creature* creatures, Heroes* heroes, Research* researches, simulasyon* senaryo,char medeniyetler[2][50],FILE *file, int birim_sayisi) {
    memset(senaryo, 0, sizeof(senaryo));
    Firstvalue firstvalues[2];
    memset(firstvalues, 0, sizeof(firstvalues));

    for (int m = 0; m < 2; m++) {
        for (int i = 0; i < 2; i++) {
            if (strcmp(scenarios[i].medeniyet, medeniyetler[m]) == 0) {

                strcpy(senaryo[m].medeniyet, scenarios[i].medeniyet);
                strcpy(firstvalues[m].medeniyet, scenarios[i].medeniyet);

                int min_kahraman_canavar;
                if (scenarios[i].kahraman_sayisi < scenarios[i].canavar_sayisi)
                    min_kahraman_canavar = scenarios[i].kahraman_sayisi;
                else
                    min_kahraman_canavar = scenarios[i].canavar_sayisi;

                senaryo[m].kahraman_sayisi = min_kahraman_canavar;
                for (int j = 0; j < senaryo[m].kahraman_sayisi; j++) {
                    strcpy(senaryo[m].kahramanlar[j], scenarios[i].kahramanlar[j]);
                    for(int k=0;k<11;k++) {
                        if(strcmp(scenarios[i].kahramanlar[j], heroes[k].heroes_ad) == 0)
                            strcpy(senaryo[m].kahraman_aciklama[j], heroes[k].heroes_aciklama);
                    }
                }

                senaryo[m].canavar_sayisi = min_kahraman_canavar;
                for (int j = 0; j < senaryo[m].canavar_sayisi; j++) {
                    strcpy(senaryo[m].canavarlar[j], scenarios[i].canavarlar[j]);
                    for(int k=0;k<11;k++) {
                        if(strcmp(scenarios[i].canavarlar[j], creatures[k].creature_ad) == 0)
                            strcpy(senaryo[m].canavar_aciklama[j], creatures[k].creature_aciklama);
                    }
                }

                for (int j = 0; j < scenarios[i].arastirma_sayisi; j++) {
                    strcpy(senaryo[m].arastirma_ad[j], scenarios[i].arastirma_turu[j]);
                    senaryo[m].arastirma_seviye[j] = scenarios[i].arastirma_seviyesi[j];
                    for(int k=0;k<11;k++) {
                        if(strcmp(scenarios[i].arastirma_turu[j], researches[k].research_ad) == 0) {
                            if(strcmp(scenarios[i].arastirma_seviye_deger[j], researches[k].research_seviye) == 0)
                                strcpy(senaryo[m].arastirma_aciklama[j], researches[k].research_aciklama);
                        }
                    }
                }
            }
        }
    }

    for (int m = 0; m < 2; m++) {
        strcpy(senaryo[m].medeniyet, medeniyetler[m]);
        senaryo[m].toplam_can = 0;
        senaryo[m].toplam_savunma = 0;
        senaryo[m].toplam_saldiri = 0;

        for (int i = 0; i < 2; i++) {
            if (strcmp(scenarios[i].medeniyet, medeniyetler[m]) == 0) {

                for (int j = 0; j < 5; j++) {
                    if (scenarios[i].birim_adetleri[j] > 0) {
                        for (int k = 0; k < birim_sayisi; k++) {
                            if (strcmp(scenarios[i].birim_adlari[j], units[k].unit_ad) == 0) {
                                int birim_can = units[k].unit_saglik;
                                int birim_savunma = units[k].unit_savunma;
                                int birim_saldiri = units[k].unit_saldiri;
                                float birim_kritik = units[k].unit_kritik_sans;
                                firstvalues[m].birim_canlari[j] = birim_can;
                                firstvalues[m].birim_savunmalari[j] = birim_savunma;
                                firstvalues[m].birim_saldirilari[j] = birim_saldiri;
                                firstvalues[m].birim_kritik_yuzdeleri[j] = birim_kritik;

                                for (int c = 0; c < scenarios[i].canavar_sayisi; c++) {
                                    for (int ci = 0; ci < 50; ci++) {
                                        if (strcmp(scenarios[i].canavarlar[c], creatures[ci].creature_ad) == 0 &&
                                            strcmp(creatures[ci].creature_etkilenen_birim, units[k].unit_ad) == 0) {
                                            strcpy(senaryo[m].canavar_buff_birimleri[c] , units[k].unit_ad);
                                            // Apply creature effects
                                            if (strcmp(creatures[ci].creature_etki_turu, "saldiri") == 0)
                                                birim_saldiri += birim_saldiri * creatures[ci].creature_etki_degeri / 100;
                                            else if (strcmp(creatures[ci].creature_etki_turu, "savunma") == 0)
                                                birim_savunma += birim_savunma * creatures[ci].creature_etki_degeri / 100;
                                            else if (strcmp(creatures[ci].creature_etki_turu, "kritik_sans") == 0)
                                                birim_kritik = ((creatures[ci].creature_etki_degeri+100.0)*birim_kritik)/100.0;
                                        }
                                    }
                                }

                                for (int h = 0; h < scenarios[i].kahraman_sayisi; h++) {
                                    for (int hero_idx = 0; hero_idx < 50; hero_idx++) {
                                        if (strcmp(scenarios[i].kahramanlar[h], heroes[hero_idx].heroes_ad) == 0) {
                                            if (strcmp(heroes[hero_idx].heroes_etkilenen_birim, units[k].unit_ad) == 0 || strcmp(heroes[hero_idx].heroes_etkilenen_birim, "tum_birimler") == 0) {
                                                strcpy(senaryo[m].kahraman_buff_birimleri[h],units[k].unit_ad);
                                                if (strcmp(heroes[hero_idx].heroes_bonus_turu, "saldiri") == 0)
                                                    birim_saldiri += birim_saldiri * heroes[hero_idx].heroes_bonus_degeri / 100;
                                                else if (strcmp(heroes[hero_idx].heroes_bonus_turu, "savunma") == 0)
                                                    birim_savunma += birim_savunma * heroes[hero_idx].heroes_bonus_degeri / 100;
                                                else if (strcmp(heroes[hero_idx].heroes_bonus_turu, "kritik_sans") == 0)
                                                    birim_kritik = ((heroes[hero_idx].heroes_bonus_degeri+100.0)*birim_kritik)/100.0;
                                            }
                                        }
                                    }
                                }

                                for (int r = 0; r < scenarios[i].arastirma_sayisi; r++) {
                                    for (int res = 0; res < 13; res++) {
                                        if (strcmp(scenarios[i].arastirma_turu[r], researches[res].research_ad) == 0 &&
                                            strcmp(scenarios[i].arastirma_seviye_deger[r], researches[res].research_seviye) == 0) {
                                            if (strcmp(researches[res].etkilenen_birim, "tum_birimler") == 0 || strcmp(researches[res].etkilenen_birim, units[k].unit_ad) == 0) {

                                                if (strcmp(researches[res].research_turu, "saldiri") == 0)
                                                    birim_saldiri += birim_saldiri * researches[res].research_deger / 100;
                                                else if (strcmp(researches[res].research_turu, "savunma") == 0)
                                                    birim_savunma += birim_savunma * researches[res].research_deger / 100;
                                                else if (strcmp(researches[res].research_turu, "kritik") == 0)
                                                    birim_kritik = ((researches[res].research_deger+100.0)*birim_kritik)/100.0;
                                            }
                                        }
                                    }
                                }

                                int index = 0;
                                while (index < 10 && senaryo[m].birim_sayilari[index] > 0) {
                                    index++;
                                }

                                if (index < 10) {
                                    strcpy(senaryo[m].birim_ad[index], units[k].unit_ad);
                                    strcpy(firstvalues[m].birim_ad[index], units[k].unit_ad);
                                    senaryo[m].birim_canlari[index] = birim_can;
                                    senaryo[m].birim_max_canlari[index] = birim_can;
                                    senaryo[m].birim_savunmalari[index] = birim_savunma;
                                    senaryo[m].birim_saldirilari[index] = birim_saldiri;
                                    senaryo[m].birim_kritik_yuzdeleri[index] = birim_kritik;
                                    senaryo[m].birim_sayilari[index] = scenarios[i].birim_adetleri[j];
                                    firstvalues[m].birim_sayilari[index] = scenarios[i].birim_adetleri[j];
                                    senaryo[m].toplam_can += birim_can * scenarios[i].birim_adetleri[j];
                                    senaryo[m].toplam_savunma += birim_savunma * scenarios[i].birim_adetleri[j];
                                    senaryo[m].toplam_saldiri += birim_saldiri * scenarios[i].birim_adetleri[j];
                                } else {
                                    printf("Birim Sayisi Asildi");
                                    exit(1);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (file != NULL) {
        fprintf(file, "Senaryoya Ait Bilgiler:\n");
        for (int i = 0; i < 2; i++) {
            print_scenario(&senaryo[i], file, &firstvalues[i]);
        }
        fprintf(file, "Araştırmalar:\n");
        for (int m = 0; m < 2; m++) {
            fprintf(file, "• %s:\n", senaryo[m].medeniyet);
            for (int i = 0; i < 2; i++) {
                if (strcmp(scenarios[i].medeniyet, medeniyetler[m]) == 0) {
                    for (int j = 0; j < (scenarios[i].arastirma_sayisi); j++) {
                        fprintf(file, "\to %s (Seviye: %d) %s\n", senaryo[m].arastirma_ad[j],scenarios[i].arastirma_seviyesi[j],senaryo[m].arastirma_aciklama[j]);
                    }
                }
            }
        }
        fprintf(file, "----------------------------------------\n\n");
        fclose(file);
        printf("Tum senaryolar basariyla 'savas_sim.txt' dosyasına kaydedildi.\n");
    } else
        fprintf(stderr, "Senaryo bulunamadı veya bir hata oluştu.\n");
}

//Function that runs the simulation step by step
void simulate(simulasyon* senaryo,char medeniyetler[2][50], FILE* file,int a,int b, int* current_state,int *round,int *attacker, int *defender,int *sayac,int *sayac2) {
    /*
            --------------------------------------------------***Battle simulation***--------------------------------------------------
    */
    file = fopen("savas_sim.txt", "a");
    if (file == NULL) {
        printf("Dosya acma hatasi!\n");
        exit(1);
    }

    int yorgunluk_sayaci = 0;
    float YORGUNLUK_CARPANI = 0.90;

    while (1) {

        fprintf(file, "Adım %d: %s'nin Saldırısı\n", (int)ceil((float)(*round)/2), medeniyetler[*attacker]);

        if ((int)ceil((float)(*round)/2) % 5 == 1 && (int)ceil((float)(*round)/2) != 1) {
            yorgunluk_sayaci++;

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 10; j++) {
                    if(senaryo[i].birim_sayilari[j] > 0){
                        senaryo[i].birim_saldirilari[j] *= YORGUNLUK_CARPANI;
                        senaryo[i].birim_savunmalari[j] *= YORGUNLUK_CARPANI;
                    }
                }

                senaryo[i].toplam_saldiri = 0;
                senaryo[i].toplam_savunma = 0;
                for (int j = 0; j < 10; j++) {
                    if(senaryo[i].birim_sayilari[j] > 0){
                        senaryo[i].toplam_saldiri += senaryo[i].birim_saldirilari[j] * senaryo[i].birim_sayilari[j];
                        senaryo[i].toplam_savunma += senaryo[i].birim_savunmalari[j] * senaryo[i].birim_sayilari[j];
                    }
                }

                if (senaryo[i].toplam_savunma <= 0)
                    senaryo[i].toplam_savunma = 1;

                fprintf(file,"Yorgunluk %s'ye uygulandi. Yeni saldiri: %d, Yeni savunma: %d\n",medeniyetler[i], senaryo[i].toplam_saldiri, senaryo[i].toplam_savunma);
            }
        }

        float total_attack = (float)senaryo[*attacker].toplam_saldiri;
        float total_defense = (float)senaryo[*defender].toplam_savunma;

        fprintf(file, "1. Saldırı Gücü Hesaplaması (Araştırma, Kahraman ve Canavar Etkileri Dahil):\n");
        float base_attack = 0;
        for (int i = 0; i < 10; i++) {
            if (senaryo[*attacker].birim_sayilari[i] > 0) {
                base_attack += senaryo[*attacker].birim_sayilari[i] * senaryo[*attacker].birim_saldirilari[i];
                fprintf(file, "   o %s: %d birim × %d saldırı gücü = %d\n",senaryo[*attacker].birim_ad[i],senaryo[*attacker].birim_sayilari[i],senaryo[*attacker].birim_saldirilari[i],senaryo[*attacker].birim_sayilari[i] * senaryo[*attacker].birim_saldirilari[i]);
            }
        }

        for (int i = 0; i < 10; i++) {
            if(senaryo[*attacker].birim_sayilari[i] > 0) {
                if(senaryo[*attacker].birim_kritik_yuzdeleri[i] != 0.0  && (((*sayac)*(int)(100.0 / (senaryo[*attacker].birim_kritik_yuzdeleri[i]))) * 2 - 1) == *round && *attacker == 0) {
                    float old = total_attack;
                    total_attack += (senaryo[*attacker].birim_sayilari[i] * senaryo[*attacker].birim_saldirilari[i] * 0.5);
                    fprintf(file,"Kritik Vurus: %s\nYapilan Ekleme:(%.0f) + %.0f\n", senaryo[*attacker].birim_ad[i], old, (total_attack - old));
                    (*sayac)++;
                }
            }
        }

        for (int i = 0; i < 10; i++) {
            if(senaryo[*attacker].birim_sayilari[i] > 0)
            {
                if(senaryo[*attacker].birim_kritik_yuzdeleri[i] != 0.0 && (((*sayac2)*(int)(100 / (senaryo[*attacker].birim_kritik_yuzdeleri[i]))) * 2) == *round &&  *attacker == 1) {
                    float old = total_attack;
                    total_attack += (senaryo[*attacker].birim_sayilari[i] * senaryo[*attacker].birim_saldirilari[i] * 0.5);
                    fprintf(file,"Kritik Vurus: %s\nYapilan Ekleme:(%.0f) + %.0f\n", senaryo[*attacker].birim_ad[i], old, (total_attack - old));
                    (*sayac2)++;
                }
            }
        }

        fprintf(file, "Toplam Saldırı Gücü (%s): %.0f\n\n", medeniyetler[*attacker], total_attack);
        fprintf(file, "2. Savunma Gücü Hesaplaması (%s):\n", medeniyetler[*defender]);
        for (int i = 0; i < 10; i++) {
            if (senaryo[*defender].birim_sayilari[i] > 0) {
                fprintf(file, "   o %s: %d birim × %d savunma gücü = %d\n",senaryo[*defender].birim_ad[i],senaryo[*defender].birim_sayilari[i],senaryo[*defender].birim_savunmalari[i],senaryo[*defender].birim_sayilari[i] * senaryo[*defender].birim_savunmalari[i]);
            }
        }

        fprintf(file, "Toplam Savunma Gücü (%s): %.0f\n\n", medeniyetler[*defender], total_defense);

        float net_damage = total_attack * (1.0 - (total_defense / total_attack));
        if (net_damage < 1)
            net_damage = 0;

        fprintf(file, "3. Net Hasar Hesaplaması:\n");
        fprintf(file, "   Net Hasar = %.0f × (1 - (%.0f / %.0f)) = %.2f\n\n", total_attack, total_defense, total_attack, net_damage);

        int total_damage_dealt = 0;

        fprintf(file, "4. Orantılı Hasar Dağılımı:\n");

        for (int i = 0; i < 10; i++) {
            if(senaryo[*defender].birim_sayilari[i] > 0)
            {
                int unit_defense = senaryo[*defender].birim_savunmalari[i] * senaryo[*defender].birim_sayilari[i];
                float damage_to_unit = net_damage * ((float)unit_defense  / total_defense);
                int damage_per_unit = damage_to_unit / (float)senaryo[*defender].birim_sayilari[i];

                int old_health = senaryo[*defender].birim_canlari[i];
                senaryo[*defender].birim_canlari[i] -= damage_per_unit;
                if (senaryo[*defender].birim_canlari[i] < 0) senaryo[*defender].birim_canlari[i] = 0;
                fprintf(file,"%s: Eski Can: %d, Alınan Hasar: %d, Yeni Can: %d\n",senaryo[*defender].birim_ad[i],old_health,damage_per_unit,senaryo[*defender].birim_canlari[i]);

                total_damage_dealt += (old_health - senaryo[*defender].birim_canlari[i]) * senaryo[*defender].birim_sayilari[i];

                if (senaryo[*defender].birim_canlari[i] <= 0) {
                    fprintf(file, "     %s tamamen yok edildi!\n", senaryo[*defender].birim_ad[i]);
                    senaryo[*defender].birim_sayilari[i] = 0;
                    senaryo[*defender].birim_canlari[i] = 0;
                }
            }
        }

        senaryo[*defender].toplam_can = 0;
        senaryo[*defender].toplam_savunma = 0;
        senaryo[*defender].toplam_saldiri = 0;

        for (int i = 0; i < 10; i++) {
            if (senaryo[*defender].birim_sayilari[i] > 0) {
                senaryo[*defender].toplam_can += (senaryo[*defender].birim_canlari[i] * senaryo[*defender].birim_sayilari[i]);
                senaryo[*defender].toplam_savunma += senaryo[*defender].birim_savunmalari[i] * senaryo[*defender].birim_sayilari[i];
                senaryo[*defender].toplam_saldiri += senaryo[*defender].birim_saldirilari[i] * senaryo[*defender].birim_sayilari[i];
            }
        }

        fprintf(file, "\n\n");

        if (senaryo[*defender].toplam_can <= 0) {
            fprintf(file, "\n%s ırkından hiçbir birim kalmadı. %s kazandı.\n", medeniyetler[*defender], medeniyetler[*attacker]);
            break;
        }

        int temp = *attacker;
        *attacker = *defender;
        *defender = temp;

        (*round)++;

        if(a!=1) {
            *current_state = 2;
            break;
        }

        if(b==1) {
            Sleep(250);
            break;
        }
    }
    fclose(file);
}