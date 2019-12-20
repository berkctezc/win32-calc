#include "stdafx.h"
#include "calculator.h"
#include "strsafe.h"

#define MAX_LOADSTRING 100
// Global Değişkenler:
HINSTANCE hInst;							// bulunulan aşama
WCHAR szBaslik[MAX_LOADSTRING];            // başlık kısmında ki yazi	
WCHAR anaPencere[MAX_LOADSTRING];		  // ana pencerenin class ismi
HWND hWndSonucuGoster;                   // sonuc göstermenin handle edilmesi

wstring SonucuGoster = L"";				  // sonucun gösterilmesi
int operand;                             // denklemde kullanılacak sayı
int islem = 0;							// kullanıcının sectigi islem
int mevcutToplam = 0;                  // mevcut toplam (elde var mantığı)
bool OperandSecFLAG = false;          // durum flagi; kullanıcının birinci değişkeni mi değiştirdiği yoksa başka bir işlem seçtiğini mi anlamak için
bool OperandSifirlaFLAG = false;     // durum flagi; işlem seçildikten sonra girdi ekranın boşaltılıp yeni değişkenin girilmesi için

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Hakkında(HWND, UINT, WPARAM, LPARAM);
void                islemiUygula(int);
void                SayisalGirdi(wstring);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // global stringlerin yüklenmesi
    LoadStringW(hInstance, IDS_APP_TITLE, szBaslik, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CALCULATOR, anaPencere, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    // aplikasyonun yüklenmesi/gösterilmesi
    if (!InitInstance(hInstance, nCmdShow))
    {return FALSE;}
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CALCULATOR));
    MSG msg;
    // Ana Mesaj Döngüsü:
    while (GetMessage(&msg, nullptr, 0, 0))
    {	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{TranslateMessage(&msg);
			DispatchMessage(&msg);} }
	return (int)msg.wParam;}
ATOM MyRegisterClass(HINSTANCE hInstance) //Pencere sınıfının(ana pencere) kaydedilmesini sağlar
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CALCULATOR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CALCULATOR);
    wcex.lpszClassName = anaPencere;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
		return RegisterClassExW(&wcex);}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) // bulunulan aşama global bir değişkene kaydedilir ve ana pencerede gösterilir
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(anaPencere, szBaslik, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, 0, 430, 660, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {return FALSE;}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
    return TRUE;
}

//	WndProc(HWND, UINT, WPARAM, LPARAM) - sonuç panelini ve tuşları pencerede yaratır
//  WM_CREATE   - tuşları yükler
//  WM_COMMAND  - menüyü yükler
//  WM_PAINT    - ana pencereyi çizer
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message)
    {case WM_CREATE: {	
        // 1.satır - sonuç ekranı
        hWndSonucuGoster = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"0", WS_CHILD | WS_VISIBLE | ES_READONLY,
            15, 10, 390, 80, hWnd, (HMENU)IDC_RESULT, GetModuleHandle(NULL), NULL);
        // 2.satır - işlem tuşları
        CreateWindowEx(NULL, L"BUTTON", L"+", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            15, 100, 90, 90, hWnd, (HMENU)IDC_ADD, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"-", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            115, 100, 90, 90, hWnd, (HMENU)IDC_SUB, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"*", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            215, 100, 90, 90, hWnd, (HMENU)IDC_MULT, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"/", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            315, 100, 90, 90, hWnd, (HMENU)IDC_DIV, GetModuleHandle(NULL), NULL);
        // 3.satır - 7,8,9
        CreateWindowEx(NULL, L"BUTTON", L"7", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            15, 200, 90, 90, hWnd, (HMENU)IDC_7, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"8", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            115, 200, 90, 90, hWnd, (HMENU)IDC_8, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"9", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            215, 200, 90, 90, hWnd, (HMENU)IDC_9, GetModuleHandle(NULL), NULL);
        // 4.satır - 4,5,6
        CreateWindowEx(NULL, L"BUTTON", L"4", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            15, 300, 90, 90, hWnd, (HMENU)IDC_4, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"5", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            115, 300, 90, 90, hWnd, (HMENU)IDC_5, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"6", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            215, 300, 90, 90, hWnd, (HMENU)IDC_6, GetModuleHandle(NULL), NULL);
        // 5.satır - 1,2,3
        CreateWindowEx(NULL, L"BUTTON", L"1", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            15, 400, 90, 90, hWnd, (HMENU)IDC_1, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"2", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            115, 400, 90, 90, hWnd, (HMENU)IDC_2, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"3", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            215, 400, 90, 90, hWnd, (HMENU)IDC_3, GetModuleHandle(NULL), NULL);
        // 6.satır - temizle,0,pozitif negatif,=
        CreateWindowEx(NULL, L"BUTTON", L"temizle", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            212, 500, 190, 90, hWnd, (HMENU)IDC_CLEAR, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            115, 500, 90, 90, hWnd, (HMENU)IDC_0, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"+/-", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            15, 500, 90, 90, hWnd, (HMENU)IDC_PLUS_MINUS, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"=", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            315, 200, 90, 290, hWnd, (HMENU)IDC_EQ, GetModuleHandle(NULL), NULL);
    }
    case WM_COMMAND:{
        int wmId = LOWORD(wParam);
        // menü seçimini ayrıştırma
        switch (wmId){
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, Hakkında);
            break;
        case IDC_ADD:
        case IDC_SUB:
        case IDC_MULT:
        case IDC_DIV:
        case IDC_EQ:
        case IDC_PLUS_MINUS:
            islemiUygula(wmId);
            break;
        case IDC_0:
            SayisalGirdi(STR_0);
            break;
        case IDC_1:
            SayisalGirdi(STR_1);
            break;
        case IDC_2:
            SayisalGirdi(STR_2);
            break;
        case IDC_3:
            SayisalGirdi(STR_3);
            break;
        case IDC_4:
            SayisalGirdi(STR_4);
            break;
        case IDC_5:
            SayisalGirdi(STR_5);
            break;
        case IDC_6:
            SayisalGirdi(STR_6);
            break;
        case IDC_7:
            SayisalGirdi(STR_7);
            break;
        case IDC_8:
            SayisalGirdi(STR_8);
            break;
        case IDC_9:
            SayisalGirdi(STR_9);
            break;
        case IDC_CLEAR:
            OperandSifirlaFLAG = false;
            OperandSecFLAG = false;
            islem = 0;
            mevcutToplam = 0;
            SonucuGoster = L"";
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);}
        // kullanıcı işlem tuşuna bastığından dolayı yeni girilen değeri ikinci değere atamak için
        if (OperandSifirlaFLAG){
            SonucuGoster = to_wstring(mevcutToplam);}
        // operand değerini kullanıcı girdisi belirler
        else if (OperandSecFLAG){
            operand = _wtoi(SonucuGoster.c_str());
            SonucuGoster = to_wstring(operand);}
        else {
            mevcutToplam = _wtoi(SonucuGoster.c_str());
            SonucuGoster = to_wstring(mevcutToplam);}
        // bütüm durumlarda sonuç ekranını günceller
        SendMessage(hWndSonucuGoster, WM_SETTEXT, NULL, (LPARAM)SonucuGoster.c_str());
    }
    break;
    case WM_PAINT:
    {	PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);}
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// aboutbox
INT_PTR CALLBACK Hakkında(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message){
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;}
        break;}
    return (INT_PTR)FALSE;}
void SayisalGirdi(wstring num) {
    if (OperandSifirlaFLAG) {
        SonucuGoster = L"";
        OperandSifirlaFLAG = false;}
    SonucuGoster.append(num);}
void islemiUygula(int op) {
	// + ve - islemleri için özel durum
	if (op == IDC_PLUS_MINUS) {
		mevcutToplam *= -1;
		return;}
	int displayNumber = _wtoi(SonucuGoster.c_str()); // editbox'un içeriğinin önizlenmesi
	// önceki işlem yok
	// operatörün işlemi yapıp kaydetmesi
	if (islem == 0) {
		islem = op;
		mevcutToplam = displayNumber;   // ilk sayının hafızada tutulması için
		OperandSecFLAG = true;          // ikinci girdinin girilmesi için
	}
	else {
		switch (islem) {
		case IDC_ADD:
			mevcutToplam += displayNumber;
			break;
		case IDC_SUB:
			mevcutToplam -= displayNumber;
			break;
		case IDC_MULT:
			mevcutToplam *= displayNumber;
			break;
		case IDC_DIV:
			mevcutToplam /= displayNumber;
			break;
		case IDC_EQ:
			islem = 0;
			break;
		}
		islem = op;				// sıradaki iş için işlemin ayarlanması
	}
		OperandSifirlaFLAG = true;
	}