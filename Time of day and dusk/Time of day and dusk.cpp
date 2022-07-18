#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include <ctime>
#include<cmath>
#include <string>
#include <stdio.h>
#include <Windows.h>

# define pi         3.141592653589793238462643383279502884L

using namespace std;

const float dr = pi / 180;
const signed char SumH = -6; // высота солнца в гражданские сумерки
float Var0, var1, Var2, Var3, var4, Var5;
int VarB1, VarB2, dTime;
float T0, VarD1, VarD2;
// Входные параметры:
// TimeM - местное время в часах
// Longitude - долгота места наблюдения
// Latitude - широта места наблюдения
// Watch - часовой пояс
// DecretTime=0 - декретное время не учитывается
// DecretTime=1 - декретное время учитывается
// Выходные параметры:
// TimeRise_h  - время восхода (ч)
// TimeRise_m  - время восхода (мин)
// TimeSet_h  - время захода (h)
// TimeSet_m  - время захода (мин)
// BeginSum_h  - начало утренних сумерек (ч)
// BeginSum_m - начало утренних сумерек (мин)
// EndSum_h  - конец вечерних сумерек (ч)
// EndSum_m - конец вечерних сумерек (мин)
// mesg - 2 строки с сообщения о полярном дне/ночи и о сумерках всю ночь

float TimeM;
double Latitude, Longitude;
int Watch, Year = 2020;
char  Mon, Day, Hour, Mins;
char DecretTime;
float RaSun[2], DecSun[2];
float TimeRise, TimeSet, MD, SideralTime, BeginSum, EndSum;
int TimeRise_h, TimeRise_m, TimeSet_h, TimeSet_m, BeginSum_h, BeginSum_m, EndSum_h, EndSum_m;
string Name_City;
float Abs(float x)
{
    return(x < 0 ? -x : x);
}

int Trunc(float x)
{
    if (x >= 0) { x = floor(x); }
    else { x = ceil(x); }
    return (long int)x;
}
// Разложение часов на часы, минуты, секунды
void Hour_hm(float TT)
{
    Hour = Trunc(TT);
    var1 = (TT - Hour) * 60;
    Mins = Trunc(var1);
}

// Приведение времени в границы 0-24 h
float Time24(float TT)
{
    if (TT >= 24) { return  TT - 24; }
    else
        if (TT < 0) { return  TT + 24; }
        else return TT;
}

// Вычисление юлианской даты модифицированной
float MJdata(int Year1, char Mon1, char Day1, float UT)
{
    if (Mon1 <= 2)
    {
        Mon1 = Mon1 + 12;
        Year1 = Year1 - 1;
    }
    VarD2 = Year1 / 400 - Year1 / 100 + Year1 / 4;
    VarD1 = 365. * Year1 - 679004.;
    return VarD1 + VarD2 + 306001 * (Mon1 + 1) / 10000 + Day1 + UT / 24;
}
// Определение дат перевода на летнее и зимнее время
void CorrectionDay(void)
{
    int day1;
    // Дата летнего времени
    Var3 = MJdata(2009, 1, 5, 0); // этот понедельник принят за 0-пункт
    var4 = MJdata(Year, 3, 31, 0); // 31 march
    Var5 = Abs(var4 - Var3 + 1);
    day1 = Trunc((Var5 / 7 - Trunc(Var5 / 7)) * 7 + 0.5);
    if (var4 - Var3 < 0) { day1 = 7 - day1; }
    VarB1 = 31 - day1; // последнее воскресенье марта
    //------
    var4 = MJdata(Year, 10, 31, 0); // 31 october
    Var5 = Abs(var4 - Var3 + 1);
    day1 = Trunc((Var5 / 7 - Trunc(Var5 / 7)) * 7 + 0.5);
    if (var4 - Var3 < 0) { day1 = 7 - day1; }
    VarB2 = 31 - day1; // последнее воскресенье октября
}
// вычисление всемирного времени (в часах) и юлианской даты
float UniversalTime_MD(void)
{
    Var0 = MJdata(Year, Mon, Day, 0);
    dTime = -Watch - DecretTime;
    var1 = Var0 + (TimeM + dTime) / 24;
    // летнее время
    CorrectionDay();
    Var3 = Time24(2. + dTime);
    var4 = MJdata(Year, 3, VarB1, 0) + Var3 / 24;
    Var3 = Time24(3. + dTime);
    Var5 = MJdata(Year, 10, VarB2, 0) + Var3 / 24;
    //if ((var1 > var4) && (var1 < Var5)) { dTime = dTime - 1; } // Летнее время
    return Var0 + (TimeM + dTime) / 24;
}
// Вычисление восхода-захода, сумерек
void SunRise(void)
{
    float L, D, ra, dec, Hh, Hrefr;
    int i;
    L = Latitude * dr; // перевод в радианы
    D = DecSun[0] * dr;
    // Проверка на полярный день
    if (DecSun[0] >= 90 - Latitude - 51 * 1.0 / 60)
    {
        //   mesg[0] = "Полярный день. Солнце не заходит"
        //   mesg[1] = " ";
        TimeRise = -1;
        BeginSum = -1;
    }
    // Проверка на полярную ночь
    if (DecSun[0] <= -90 + Latitude - 51 * 1.0 / 60)
    {
        //   mesg[0] = "Полярная ночь. Солнце не восходит"
        TimeRise = -1;
        TimeRise = -1; // восход-заход вычисляться не будет
    }
    //
    if (DecSun[0] >= 90 - Latitude - 51 * 1.0 / 60 + SumH)
    {
        //    mesg[1] = "Гражданские сумерки всю ночь"
        BeginSum = -1;
        EndSum = -1; // сумерки вычисляться не будут
    }
    //------------------Начинаем вычисления----------------
    var1 = (RaSun[1] - RaSun[0]) / 24; // dra=var1
    Var2 = (DecSun[1] - DecSun[0]) / 24; // ddec=var2
    Var3 = SideralTime - RaSun[0]; // th=var3
    var4 = sin(L) * sin(D) + cos(L) * cos(D) * cos(Var3 * dr);
    Hh = 90 - acos(var4) / dr; // Это начальное значение высоты во всемирную полночь
    VarB1 = 4; // шаг цикла
    for (i = 0; i < 23 * VarB1 + VarB1 + 1; i++)
    {
        Var3 = Hh;
        ra = RaSun[0] + i * var1 / VarB1;
        dec = DecSun[0] + i * Var2 / VarB1;
        Var5 = SideralTime + i * 15.0 * 1.002738 / VarB1;
        var4 = sin(L) * sin(dec * dr) + cos(L) * cos(dec * dr) * cos((Var5 - ra) * dr);
        Hh = 90 - acos(var4) / dr;
        Hrefr = Hh + 51 * 1.0 / 60; // с учетом рефракции
        if ((TimeRise != -1) && (Var3 < 0) && (Hh > 0)) { TimeRise = i * 1.0 / VarB1 - Hrefr / (Hh - Var3) / VarB1; }
        if ((TimeRise != -1) && (Var3 > 0) && (Hh < 0)) { TimeSet = i * 1.0 / VarB1 - Hrefr / (Hh - Var3) / VarB1; }
        if ((BeginSum != -1) && (Var3 <= SumH) && (Hh >= SumH)) { BeginSum = i * 1.0 / VarB1 + (SumH - Hh) / (Hh - Var3) / VarB1; }
        if ((EndSum != -1) && (Var3 >= SumH) && (Hh <= SumH)) { EndSum = i * 1.0 / VarB1 + (SumH - Hh) / (Hh - Var3) / VarB1; }
    }
    if (TimeRise != -1) { TimeRise = Time24(TimeRise - dTime); }
    if (TimeSet != -1) { TimeSet = Time24(TimeSet - dTime); }
    if (BeginSum != -1) { BeginSum = Time24(BeginSum - dTime); }
    if (EndSum != -1) { EndSum = Time24(EndSum - dTime); }
}
// вычисление углов из вектора
void Angles_from_Vector(float R[], float* ra, float* dec)
{
    *ra = atan2(R[1], R[0]) / dr;
    *dec = atan2(R[2], sqrt(R[0] * R[0] + R[1] * R[1])) / dr;
    if (*ra < 0) { *ra = *ra + 360; }
}
//  Вычисление звездного времени (среднее гринвическое)
float JD_Sideral_Grinvich(float MD)
{
    float S0;
    Var0 = Trunc(MD);
    T0 = (Var0 - 51544.5) / 36525;
    S0 = 24110.54841 + 8640184.812 * T0 + 0.093104 * T0 * T0 - 0.0000062 * T0 * T0 * T0;  // зв.время гринвич на полночь даты
    var1 = (MD - Var0) * 86400;
    S0 = (S0 + var1 * 1.002738) / 3600 * 15; // in degree
    while (S0 >= 360) { S0 = S0 - 360; }
    return S0;
}
// Вычисление эклиптических и экваториальных к-т Солнца
void SunPoz(float MD, float sun[])
{
    float hvect[3];
    T0 = (Trunc(MD) - 51544.5) / 36525;
    Var0 = (MD - Trunc(MD)) * 24;
    VarD1 = 357.528 + 35999.05 * T0 + 0.04107 * Var0;
    VarD2 = 280.46 + 36000.772 * T0 + 0.04107 * Var0;
    VarD2 = VarD2 + (1.915 - 0.0048 * T0) * sin(VarD1 * dr) + 0.02 * sin(2 * VarD1 * dr);
    hvect[0] = cos(VarD2 * dr);
    hvect[1] = sin(VarD2 * dr);
    hvect[2] = 0;
    var1 = 23.439281 * dr;
    sun[0] = hvect[0];
    sun[1] = hvect[1] * cos(var1) - hvect[2] * sin(var1);
    sun[2] = hvect[1] * sin(var1) + hvect[2] * cos(var1);
}

void Calculation(void)
{
    float JulDat, sun1[3], sun2[3];
    //Watch = 0;
    Hour = 12;
    Mins = 0;
    DecretTime = 0;
    TimeRise = 0;
    TimeSet = 0;
    BeginSum = 0;
    EndSum = 0;
    //
    TimeM = Hour + Mins / 60;
    // переходим ко всемирному времени и определяем поправку (летнее время+часовое+декретное)
    JulDat = UniversalTime_MD();
    MD = Trunc(JulDat); // юлианская дата на полночь
    SideralTime = JD_Sideral_Grinvich(MD) + Longitude; // местное звездное время на всемирную полночь
                                // оно будет востребовано при вычислении азимутальных координат солнца
    // положение солнца в полночь даты
    SunPoz(MD, sun1);
    Angles_from_Vector(sun1, &RaSun[0], &DecSun[0]); // экваториальные геоц. координаты Солнца RaSun[0], DecSun[0]
    // положение солнца через сутки
    SunPoz(MD + 1, sun2);
    Angles_from_Vector(sun2, &RaSun[1], &DecSun[1]); // экваториальные геоц. координаты Солнца RaSun[1], DecSun[1]
    // восход-заход
    SunRise();
    // на выходе процедуры SunRise время в часах
    if (TimeRise != -1)
    {
        Hour_hm(TimeRise);
        TimeRise_h = Time24(Hour); TimeRise_m = Mins;
        Hour_hm(TimeSet);
        TimeSet_h = Time24(Hour); TimeSet_m = Mins;

    }
    else TimeRise_h = -1; // значит, полярная ночь в разгаре
//
    if (BeginSum != -1)
    {
        Hour_hm(BeginSum);
        if (Hour > 24) { Hour = Hour - 24; }
        BeginSum_h = Hour; BeginSum_m = Mins;
        Hour_hm(EndSum);
        if (Hour > 24) { Hour = Hour - 24; }
        EndSum_h = Hour; EndSum_m = Mins;
    }
    else BeginSum_h = -1; // значит всю ночь сумерки
}

map<string, map<int, map<string, map<int, vector<string>>>>> data_preparation()
{
    map<int, vector<string>> january;
    map<int, vector<string>> february;
    map<int, vector<string>> march;
    map<int, vector<string>> april;
    map<int, vector<string>> may;
    map<int, vector<string>> june;
    map<int, vector<string>> july;
    map<int, vector<string>> august;
    map<int, vector<string>> september;
    map<int, vector<string>> october;
    map<int, vector<string>> november;
    map<int, vector<string>> december;

    for (int i = 1; i <= 28; i++) {
        february[i] = { "0","0","0","0" };
    }
    for (int i = 1; i <= 30; i++) {
        april[i] = { "0","0","0","0" };
        june[i] = { "0","0","0","0" };
        september[i] = { "0","0","0","0" };
        november[i] = { "0","0","0","0" };
    }
    for (int i = 1; i <= 31; i++) {
        january[i] = { "0","0","0","0" };
        march[i] = { "0","0","0","0" };
        may[i] = { "0","0","0","0" };
        july[i] = { "0","0","0","0" };
        august[i] = { "0","0","0","0" };
        october[i] = { "0","0","0","0" };
        december[i] = { "0","0","0","0" };
    }
    map<string, map<int, vector<string>>> january_numbered = { {"Январь",january} };
    map<string, map<int, vector<string>>> february_numbered = { {"Февраль",february} };
    map<string, map<int, vector<string>>> march_numbered = { {"Март",march} };
    map<string, map<int, vector<string>>> april_numbered = { {"Апрель",april} };
    map<string, map<int, vector<string>>> may_numbered = { {"Май",may} };
    map<string, map<int, vector<string>>> june_numbered = { {"Июнь",june} };
    map<string, map<int, vector<string>>> july_numbered = { {"Июль",july} };
    map<string, map<int, vector<string>>> august_numbered = { {"Август",august} };
    map<string, map<int, vector<string>>> september_numbered = { {"Сентябрь",september} };
    map<string, map<int, vector<string>>> october_numbered = { {"Октябрь",october} };
    map<string, map<int, vector<string>>> november_numbered = { {"Ноябрь",november} };
    map<string, map<int, vector<string>>> december_numbered = { {"Декабрь",december} };
    map<int, map<string, map<int, vector<string>>>>not_leap = { { 1,january_numbered} ,{ 2,february_numbered},
        { 3,march_numbered}, { 4,april_numbered}, { 5,may_numbered}, { 6,june_numbered}, { 7,july_numbered}, { 8,august_numbered},
        { 9,september_numbered}, { 10,october_numbered}, { 11,november_numbered}, { 12,december_numbered} };
    february[29] = { "0","0","0","0" };
    february_numbered = { {"Февраль",february} };
    map<int, map<string, map<int, vector<string>>>>leap = { { 1,january_numbered} ,{ 2,february_numbered},
        { 3,march_numbered}, { 4,april_numbered}, { 5,may_numbered}, { 6,june_numbered}, { 7,july_numbered}, { 8,august_numbered},
        { 9,september_numbered}, { 10,october_numbered}, { 11,november_numbered}, { 12,december_numbered} };
    return { {"Високосный",leap},{"Невисокосный",not_leap } };
}

void time_voshode(map<string, map<int, map<string, map<int, vector<string>>>>>& data) {
    for (auto& leaps : data) {
        for (auto& number_of_month : leaps.second) {
            for (auto& name_of_month : number_of_month.second) {
                for (auto& days : name_of_month.second) {//days.first - номер дня;days.second - вектор значений
                    Day = days.first;
                    Mon = number_of_month.first;
                    Calculation();
                    string TimeRise_h_S, TimeRise_m_S, TimeSet_h_S, TimeSet_m_S, BeginSum_h_S, BeginSum_m_S, EndSum_h_S, EndSum_m_S;
                    string polar = "полярная ситуация";
                    if (BeginSum_h == -1) BeginSum_h_S = polar;
                    else if (BeginSum_h < 10) BeginSum_h_S = "0" + to_string(BeginSum_h);
                    else BeginSum_h_S = to_string(BeginSum_h);
                    if (BeginSum_m == -1) BeginSum_m_S = polar;
                    else if (BeginSum_m < 10)BeginSum_m_S = "0" + to_string(BeginSum_m);
                    else BeginSum_m_S = to_string(BeginSum_m);
                    if (TimeRise_h == -1) TimeRise_h_S = polar;
                    else if (TimeRise_h < 10)TimeRise_h_S = "0" + to_string(TimeRise_h);
                    else TimeRise_h_S = to_string(TimeRise_h);
                    if (TimeRise_m == -1) TimeRise_m_S = polar;
                    else if (TimeRise_m < 10)TimeRise_m_S = "0" + to_string(TimeRise_m);
                    else TimeRise_m_S = to_string(TimeRise_m);
                    if (TimeSet_h == -1)TimeSet_h_S = polar;
                    else if (TimeSet_h < 10) TimeSet_h_S = "0" + to_string(TimeSet_h);
                    else TimeSet_h_S = to_string(TimeSet_h);
                    if (TimeSet_m == -1)TimeSet_m_S = polar;
                    else if (TimeSet_m < 10)TimeSet_m_S = "0" + to_string(TimeSet_m);
                    else TimeSet_m_S = to_string(TimeSet_m);
                    if (EndSum_h == -1) EndSum_h_S = polar;
                    else if (EndSum_h < 10)EndSum_h_S = "0" + to_string(EndSum_h);
                    else EndSum_h_S = to_string(EndSum_h);
                    if (EndSum_m == -1) EndSum_m_S = polar;
                    else if (EndSum_m < 10)EndSum_m_S = "0" + to_string(EndSum_m);
                    else EndSum_m_S = to_string(EndSum_m);

                    if (BeginSum_h_S == polar)
                    {
                        days.second[0] = polar;
                        days.second[3] = polar;
                    }
                    else
                    {
                        days.second[0] = BeginSum_h_S + "." + BeginSum_m_S;
                        days.second[3] = EndSum_h_S + "." + EndSum_m_S;
                    }

                    if (TimeRise_h_S == polar)
                    {
                        days.second[1] = polar;
                        days.second[2] = polar;
                    }
                    else
                    {
                        days.second[1] = TimeRise_h_S + "." + TimeRise_m_S;
                        days.second[2] = TimeSet_h_S + "." + TimeSet_m_S;
                    }
                }
            }
        }
        Year++;
    }
}
void recording_data(map<string, map<int, map<string, map<int, vector<string>>>>> data) {
    ofstream file;
    file.open("table_sun\\" + Name_City + ".ini");
    if (file.is_open())
    {
        string Watch_s;
        if (Watch < 0)Watch_s = "-" + to_string(Watch);
        else Watch_s = "+" + to_string(Watch);
        //file << "\"Введенная информация\"\n";
        //file << "{\n\"Название города\"=\"" << Name_City << "\"\n";
        //file << "\"Часовой пояс\"=\"(GMT" << Watch_s << ")\"\n";
        //file << "\"Широта\"=\"" << Latitude << "\"\n";
        //file << "\"Долгота\"=\"" << Longitude << "\"\n}\n";
        for (const auto& leaps : data) {
            file << "\"" << leaps.first << "\"\n";
            file << "{\n";
            for (const auto& number_of_month : leaps.second) {
                for (const auto& name_of_month : number_of_month.second) {
                    file << "  " << "\"" << name_of_month.first << "\"\n  {\n";
                    for (const auto& days : name_of_month.second) {//days.first - номер дня;days.second - вектор значений
                        file << "   \"" << days.first << "\"\n   {\n";
                        file << "     \"День начало\"=\"" << days.second[1] << "\"\n";
                        file << "     \"День конец\"=\"" << days.second[2] << "\"\n";
                        file << "     \"Сумерки конец\"=\"" << days.second[3] << "\"\n";
                        file << "     \"Сумерки начало\"=\"" << days.second[0] << "\"\n";
                        file << "   }\n";
                    }
                    file << "  }\n";
                }
            }
            file << "}\n";
        }
    }
    file.close();
}
int main()
{
    //setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
    map<string, map<int, map<string, map<int, vector<string>>>>> data = data_preparation();
    Name_City = "ЧерскийМой";
    Watch = 0;
    Latitude = 68.7516666666666666666666666666666667;
    Longitude = 161.3297222222222222222222222222;

    //cout << "Введите Город(одним словом, без пробелов): ";
    //cin >> Name_City;
    //cout << "Введите часовой пояс(знак направления и целое значение пояса): ";
    //cin >> Watch;
    //cout << "Введите широту(без минут и секунд, в долях градуса): ";
    //cin >> Latitude;
    //cout << "Введите долготу(без минут и секунд, в долях градуса): ";
    //cin >> Longitude;
    time_voshode(data);
    recording_data(data);
}