#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void printMessage(QString str) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Информационное сообщение");
    msgBox.setText(str);
    msgBox.setIcon(QMessageBox::Information); // Установите иконку
    msgBox.setStandardButtons(QMessageBox::Ok); // Кнопка "ОК"
    msgBox.exec(); // Показать сообщение
}

void Encrypt_Decrypt(const char* file_to_encrypt, string &str, int &user_choice)
{
    ofstream WriteStream;//открываем поток на запись
    WriteStream.open("encrypted_text.txt", ios::app);//для шифровки, поток открыт на дозапись
    if (user_choice == 2)
    {
        WriteStream.close();
        WriteStream.open("decrypted_text.txt", ios::app);//для расшифровки
    }
    //проверяем количество слов в текте файла
    const string separators{ " \n" }; //строка разделителей
    vector <string> words; //вектор в котором будут храниться слова из текста
    size_t start{ str.find_first_not_of(separators) }; // начальный индекс первого слова
    while (start != string::npos) // проходим, пока в строке не окажется других символов, кроме separators
    {
        size_t end = str.find_first_of(separators, start + 1); // находим, где кончается слово
        if (end == string::npos) // если НЕ найден ни один из символов-разделителей
        {
            end = str.length();        // устанавливаем переменную end на конец текста
        }
        words.push_back(str.substr(start, end - start)); // добавляем в вектор слово
        start = str.find_first_not_of(separators, end + 1); // находим начальный индекс следующего слова и переустанавливаем start
    }
    int vector_length = words.size();//длина вектора
    //перемещение по словам в векторе
    for (int j = 0; j < vector_length; j++)
    {
        if (j > 0)
        {
            WriteStream << " ";//запись пробелов в текст после первого слова
        }
        int word_length = words[j].length();//длина слова
        //цикл посимвольного шифрования
        for (int i = 0; i < word_length; i++)
        {
            unsigned char enc = static_cast<unsigned char>(words[j][i]);//определяем символ для шифрования
            if (user_choice == 1)
            {
                enc += j + 1;//определяем ключ шифрования в данном слове и шифруем
            }
            else
            {
                enc -= j + 1;//определяем ключ шифрования в данном слове и дешифруем
            }
            WriteStream << enc;//запись в файл
        }
        if (j == vector_length - 1)
        {
            WriteStream << "\n";//если слово является концом строки, то мы переносим строку
        }
    }
    if (user_choice == 1) {
        printMessage("Текст из выбранного файла\nзашифрован и сохранён в файл encrypted_text.txt");
    } else {
        printMessage("Текст из выбранного файла\nрасшифрован и сохранён в файл decrypted_text.txt");
    }
    WriteStream.close();//закрываем поток на запись
}


void ReadStringFromFile(const char* file_to_encrypt, int &user_choice)
{
    string str;
    //открываем поток на чтение из выбранного файла
    ifstream ReadStream;
    ReadStream.open(file_to_encrypt, ios::in);//чтение построчно

    if (user_choice == 2)
    {
        ReadStream.close();
        ReadStream.open(file_to_encrypt, ios::in);
    }

    //проверяем открыт ли поток на чтение, если да, то читаем файл построчно и передаём эти строки в функцию шифрования
    //если нет, то так и говорим, в интерфейсе там будет message box ну или что-то типа него
    if (ReadStream.is_open())
    {
        //int encrypt_key;
        while (getline(ReadStream, str))
        {
            Encrypt_Decrypt(file_to_encrypt, str, user_choice);
        }
    }
    else
    {
        printMessage("Не выбран файл.");
    }
    ReadStream.close();
    exit;
}

void MainWindow::on_btn_crypt_clicked()
{
    QString filePath = ui->lineEdit_choose->text();
    const char* file_to_encrypt = filePath.toUtf8().constData();

    int user_choice = 1;

    ofstream out("encrypted_text.txt", ios::out);//очистка шифруемого файла
    out.close();

    ReadStringFromFile(file_to_encrypt, user_choice);
}


void MainWindow::on_btn_choose_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Все файлы (*)");
    ui->lineEdit_choose->setText(fileName);
}


void MainWindow::on_btn_decrypt_clicked()
{
    QString filePath = ui->lineEdit_choose->text();
    const char* file_to_encrypt = filePath.toUtf8().constData();

    int user_choice = 2;
    ofstream out2("decrypted_text.txt", ios::out);//очистка расшифрованного файла
    out2.close();

   ReadStringFromFile(file_to_encrypt, user_choice);
}

