#include <time.h>
#include <stdbool.h>

#include "CNFGAndroid.h"

#define CNFG3D
#define CNFG_IMPLEMENTATION
#include "CNFG.h"

volatile int android_uspani;

// Šířka a výška okna
short okno_w, okno_h;

// Šířka a výška textu
int text_w, text_h;

// Makra pro získání prostředku okna
#define PROSTREDEK_W (okno_w / 2)
#define PROSTREDEK_H (okno_h / 2)

// Makro pro umístění textu doprostřed
#define PROSTREDEK_PODLE_TEXTU ((okno_w - text_w) / 2)

// Barvy
#define BILA 0xffffffff
#define CERNA 0x000000ff
#define ORANZOVA 0x999900ff
#define TMAVE_ZELENA 0x008000ff
#define ZELENA 0x009900ff
//#define CERVENA 0x990000ff
#define RUZOVA 0xff1493ff

// Možné odpovědi kyvadla
// Mimo PRAZDNO, to slouží jen pro inicializaci
typedef enum
{
    PRAZDNO,
    ANO,
    NE,
    MOZNA
} odpoved;

// Kde se právě nachází kyvadlo?
typedef enum
{
    PROSTREDEK,
    VPRAVO,
    VLEVO,
    DOLE,
    NAHORE
} pozice;

// Bude se přičítat nebo odečítat?
typedef enum
{
    MINUS,
    PLUS
} vypocet;


typedef enum
{
    NIC,
    HD,
    FULLHD
} rozliseni;

// Odpověď kyvadla
odpoved kyvadlo_odpoved = PRAZDNO;

// Odchylka pro animaci pohybu kyvadla
RDPoint kyvadlo_odchylka = {0, 0};

// Aktuální a poslední pozice kyvadla
pozice kyvadlo_pozice = PROSTREDEK;
pozice kyvadlo_pozice_predchozi;


// Rozlišení telefonu
rozliseni telefon_rozliseni;

// Animace po kliknutí
bool kliknuto = false;
pozice nadpis_pozice = NAHORE;
int  nadpis_posun = 0;


/*
 * Animace textu při kliknutí na tlačítko
 */
void Kliknuto(void) {
    if (kliknuto == true)
        switch (nadpis_pozice) {
            case NAHORE:
                nadpis_posun++;
                break;
            case DOLE:
                nadpis_posun--;
                break;
        }
    if (nadpis_posun >= 20) {
        nadpis_pozice = DOLE;
        kliknuto = false;
    }
    else if(nadpis_posun <= 0) {
        nadpis_pozice = NAHORE;
        kliknuto = false;
    }

}
/*
 * Náhodné generování ANO/NE/MOŽNÁ
 */
void Generator(void)
{
    int r;
    srand((unsigned int)time(NULL));
    r = rand() % 3;
    switch (r)
    {
    case 0:
        kyvadlo_odpoved = ANO;
        break;
    case 1:
        kyvadlo_odpoved = NE;
        break;
    case 2:
        kyvadlo_odpoved = MOZNA;
        break;
    default:
        break;
    }
}

/*
 * Modifikace proměnné odchylka
 * x: odchylka.x
 * y: odchylka.y
 * MINUS od proměnné odečte 1, PLUS k proměnné přičte 1
 */
void Odchylka(vypocet x, vypocet y);

/*
 * Stisknutí klávesy
 * Povinná funkce Rawdraw
 */
void HandleKey(__attribute__((unused)) int keycode, __attribute__((unused)) int bDown) {}

/*
 * Kliknutí v prostoru makecapk
 * Povinná funkce Rawdraw
 */
void HandleButton(int x, int y, __attribute__((unused)) int button, int bDown)
{
    // Kliknuto
    // (Funkce HandleButton proběhne pokaždé dvakrát, podruhé při puštění tlačítka, tehdy je bDown rovno nule)
    if (bDown == 1)
    {
        // Pozice tlačítka PORAD MI
        // Je nutno dělat přes body
        if (x >= PROSTREDEK_W - 150 * telefon_rozliseni && x <= PROSTREDEK_W + 150 * telefon_rozliseni)
        {
            if (y >= PROSTREDEK_H + 50 * telefon_rozliseni && y <= PROSTREDEK_H + 150 * telefon_rozliseni) {
                // Vytvoření odpovědi
                kliknuto = true;
                Generator();
            }
        }
    }
    //printf("%d\n%d\n%d\n\n", x, y, bDown);
}

/*
 * Pohyb kursoru
 * Povinná funkce Rawdraw
 */
void HandleMotion(__attribute__((unused)) int x, __attribute__((unused)) int y, __attribute__((unused)) int mask) {}

/*
 * Uzavření okna
 * Povinná funkce Rawdraw
 */
void HandleDestroy() {}

/*
 * Uspání programu
 * Povinná funkce Rawdrawandroid
 */
void HandleSuspend()
{
    android_uspani = true;
}

/*
 * Probuzení programu
 * Povinná funkce Rawdrawandroid
 */
void HandleResume()
{
    android_uspani = false;
}

int main()
{
    // Název okna, jeho výška a šířka
    CNFGSetupFullscreen("Zeptej se kyvadla", 0);
    CNFGGetDimensions(&okno_w, &okno_h);

    while (1)
    {
        CNFGHandleInput();

        if (android_uspani == true)
        {
            usleep(50000);
            continue;
        }
        // Barva pozadí
        CNFGBGColor = CERNA;

        CNFGClearFrame();

        // Aktualizace proměnné okno_w a okno_h na aktuální rozměry okna
        // Barva obsahu
        CNFGColor(BILA);

        switch (okno_w)
        {
            case 1080:
                telefon_rozliseni = FULLHD;
                break;

            case 720:
                telefon_rozliseni = HD;
                break;
            default:
                telefon_rozliseni = HD;
                break;
        }

        // Text Kyvadlo
        CNFGGetTextExtents("KYVADLO", &text_w, &text_h, 20 * telefon_rozliseni);
        CNFGPenX = PROSTREDEK_PODLE_TEXTU;
        CNFGPenY = PROSTREDEK_H - 150 * telefon_rozliseni + nadpis_posun;

        CNFGDrawText("KYVADLO", 20 * telefon_rozliseni);

        Kliknuto();

        // Vytvoření kosočtverce tvořícího závaží kyvadla
        CNFGTackSegment(PROSTREDEK_W + kyvadlo_odchylka.x * telefon_rozliseni, PROSTREDEK_H + kyvadlo_odchylka.y * telefon_rozliseni, PROSTREDEK_W, PROSTREDEK_H - 80 * telefon_rozliseni);
        RDPoint points[4] = {{PROSTREDEK_W + kyvadlo_odchylka.x * telefon_rozliseni, PROSTREDEK_H + kyvadlo_odchylka.y * telefon_rozliseni},
                             {PROSTREDEK_W - 10 + kyvadlo_odchylka.x * telefon_rozliseni, PROSTREDEK_H + 10 + kyvadlo_odchylka.y * telefon_rozliseni},
                             {PROSTREDEK_W + kyvadlo_odchylka.x * telefon_rozliseni, PROSTREDEK_H + 20 + kyvadlo_odchylka.y * telefon_rozliseni},
                             {PROSTREDEK_W + 10 + kyvadlo_odchylka.x * telefon_rozliseni, PROSTREDEK_H + 10 + kyvadlo_odchylka.y * telefon_rozliseni}};
        CNFGTackPoly(points, 4);

        // Čtverec tvořící výplň tlačítka
        CNFGColor(TMAVE_ZELENA);
        CNFGTackRectangle(PROSTREDEK_W - 150 * telefon_rozliseni, PROSTREDEK_H + 50 * telefon_rozliseni, PROSTREDEK_W + 150 * telefon_rozliseni, PROSTREDEK_H + 110 * telefon_rozliseni);

        CNFGColor(BILA);
        // Text tvořící popis tlačítka
        CNFGGetTextExtents("PORAD MI", &text_w, &text_h, 11 * telefon_rozliseni);
        CNFGPenX = PROSTREDEK_PODLE_TEXTU;
        CNFGPenY = PROSTREDEK_H + 57 * telefon_rozliseni;
        CNFGDrawText("PORAD MI", 11 * telefon_rozliseni);
        // Zobrazení odpovědi
        switch (kyvadlo_odpoved)
        {
        case MOZNA:
            CNFGGetTextExtents("MOZNA", &text_w, &text_h, 10 * telefon_rozliseni);
            CNFGPenX = PROSTREDEK_PODLE_TEXTU;
            CNFGPenY = PROSTREDEK_H + 160 * telefon_rozliseni;
            CNFGColor(ORANZOVA);

            CNFGDrawText("MOZNA", 10 * telefon_rozliseni);
            break;
        case ANO:
            CNFGGetTextExtents("BUDIZ", &text_w, &text_h,  10 * telefon_rozliseni);
            CNFGPenX = PROSTREDEK_PODLE_TEXTU;
            CNFGPenY = PROSTREDEK_H + 160 * telefon_rozliseni;
            CNFGColor(ZELENA);
            CNFGDrawText("BUDIZ", 10 * telefon_rozliseni);
            break;
        case NE:
            CNFGGetTextExtents("NIKOLIV", &text_w, &text_h, 10 * telefon_rozliseni);
            CNFGPenX = PROSTREDEK_PODLE_TEXTU;
            CNFGPenY = PROSTREDEK_H + 160 * telefon_rozliseni;
            CNFGColor(RUZOVA);
            CNFGDrawText("NIKOLIV", 10 * telefon_rozliseni);
        default:
            break;
        }

        CNFGSwapBuffers();

        // Kyvadlo je veprostřed
        if (points[0].x == PROSTREDEK_W)
        {
            kyvadlo_pozice_predchozi = kyvadlo_pozice;
            kyvadlo_pozice = PROSTREDEK;
            // Kyvadlo je v právém horním rohu
        }
        else if (points[0].x > PROSTREDEK_W + 45 * telefon_rozliseni)
            kyvadlo_pozice = VPRAVO;
        // Kyvadlo je v levém horním rohu
        else if (points[0].x < PROSTREDEK_W - 45 * telefon_rozliseni)
            kyvadlo_pozice = VLEVO;

        // Pohyb kyvadla
        switch (kyvadlo_pozice)
        {
        case PROSTREDEK:
            if (kyvadlo_pozice_predchozi == VPRAVO)
                Odchylka(MINUS, MINUS);
            else
                Odchylka(PLUS, MINUS);
            break;
        case VPRAVO:
            Odchylka(MINUS, PLUS);
            break;
        case VLEVO:
            Odchylka(PLUS, PLUS);
            break;
        default:
            break;
        }
        usleep(25000);
    }
    return 0;
}

inline void Odchylka(vypocet x, vypocet y)
{
    switch (x)
    {
    case PLUS:
        kyvadlo_odchylka.x++;
        break;
    case MINUS:
        kyvadlo_odchylka.x--;
        break;
    default:
        break;
    }
    switch (y)
    {
    case PLUS:
        kyvadlo_odchylka.y++;
        break;
    case MINUS:
        kyvadlo_odchylka.y--;
        break;
    default:
        break;
    }
}