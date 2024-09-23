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

void MainWindow::on_btn_crypt_clicked()
{
    QString filePath = ui->lineEdit_choose->text();
    const char* file_to_encrypt = filePath.toUtf8().constData();

    ifstream ReadStream(file_to_encrypt);

    QString lineEdit_first_key = ui->lineEdit_first_key->text();
    QString lineEdit_second_key = ui->lineEdit_second_key->text();

    bool check_first_key;
    bool check_second_key;

    int first_key = lineEdit_first_key.toInt(&check_first_key, 10);
    int second_key = lineEdit_second_key.toInt(&check_second_key, 10);

    string str;

    if (check_first_key == false && check_second_key == false) {
        printMessage("Вы неверно ввели оба ключа.\nПерепроверьте ключи.");
    } else if (check_first_key == false && check_second_key == true) {
        printMessage("Вы неверно ввели первый ключ.\nПерепроверьте ключ.");
    } else if (check_first_key == true && check_second_key == false) {
        printMessage("Вы неверно ввели второй ключ.\nПерепроверьте ключ.");
    } else {
        while (getline(ReadStream, str))
        {
            ofstream WriteStream("encrypted_text.txt");//открываем поток на запись

            int lenght = str.length();

            //цикл посимвольного шифрования
            for (int i = 0; i < lenght; i++)
            {
                unsigned char enc = static_cast<unsigned char>(str[i]);
                enc += first_key;
                enc += second_key;
                WriteStream << enc;
            }

            WriteStream.close();//закрываем поток на запись
        }
        printMessage("Текст из выбранного файла\nзашифрован и сохранён в файл encrypted_text.txt");
    }
}


void MainWindow::on_btn_choose_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Все файлы (*)");
    ui->lineEdit_choose->setText(fileName);
}


void MainWindow::on_btn_decrypt_clicked()
{
    QString lineEdit_first_key = ui->lineEdit_first_key->text();
    QString lineEdit_second_key = ui->lineEdit_second_key->text();

    bool check_first_key;
    bool check_second_key;

    int first_key = lineEdit_first_key.toInt(&check_first_key, 10);
    int second_key = lineEdit_second_key.toInt(&check_second_key, 10);

    QString filePath = ui->lineEdit_choose->text();
    const char* file_to_decrypt = filePath.toUtf8().constData();

    filesystem::path p(file_to_decrypt);

    if (p.filename() != "encrypted_text.txt") {
        printMessage("Вы выбрали неверный файл.\nВыберите зашифрованный файл.");
    } else {
        if (check_first_key == false && check_second_key == false) {
            printMessage("Вы неверно ввели оба ключа.\nПерепроверьте ключи.");
        } else if (check_first_key == false && check_second_key == true) {
            printMessage("Вы неверно ввели первый ключ.\nПерепроверьте ключ.");
        } else if (check_first_key == true && check_second_key == false) {
            printMessage("Вы неверно ввели второй ключ.\nПерепроверьте ключ.");
        } else {
            ifstream ReadStream(file_to_decrypt);

            if (ReadStream.is_open())
            {
                string str;

                while (getline(ReadStream, str))
                {
                    ofstream WriteStream("decrypted_text.txt");//открываем поток на запись
                    int lenght = str.length();

                    //цикл посимвольного шифрования
                    for (int i = 0; i < lenght; i++)
                    {
                        unsigned char dec = static_cast<unsigned char>(str[i]);
                        dec -= first_key;
                        dec -= second_key;
                        WriteStream << dec;
                    }

                    WriteStream.close();//закрываем поток на запись
                }

            }
            printMessage("Текст из выбранного файла\nрасшифрован и сохранён в файл decrypted_text.txt");
        }
    }
}

