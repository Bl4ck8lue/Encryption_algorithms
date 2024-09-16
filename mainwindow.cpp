#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
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

void MainWindow::on_btn_crypt_clicked()
{
    QString filePath = ui->lineEdit_choose->text();
    const char* file_to_encrypt = filePath.toUtf8().constData();

    ifstream ReadStream(file_to_encrypt);

    string str;

    QString lineEdit_first_key = ui->lineEdit_first_key->text();
    QString lineEdit_second_key = ui->lineEdit_second_key->text();

    while (getline(ReadStream, str))
    {
        ofstream WriteStream("out.txt");//открываем поток на запись

        int lenght = str.length();

        //цикл посимвольного шифрования
        for (int i = 0; i < lenght; i++)
        {
            unsigned char enc = static_cast<unsigned char>(str[i]);
            enc += lineEdit_first_key.toInt();
            enc += lineEdit_second_key.toInt();
            WriteStream << enc;
        }

        WriteStream.close();//закрываем поток на запись
    }

}


void MainWindow::on_btn_choose_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Все файлы (*)");
    ui->lineEdit_choose->setText(fileName);
}


void MainWindow::on_btn_decrypt_clicked()
{
    QString filePath = ui->lineEdit_choose->text();
    const char* file_to_decrypt = filePath.toUtf8().constData();

    ifstream ReadStream(file_to_decrypt);

    if (ReadStream.is_open())
    {
        string str;

        QString lineEdit_first_key = ui->lineEdit_first_key->text();
        QString lineEdit_second_key = ui->lineEdit_second_key->text();

        while (getline(ReadStream, str))
        {
            ofstream WriteStream("out2.txt");//открываем поток на запись
            int lenght = str.length();

            //цикл посимвольного шифрования
            for (int i = 0; i < lenght; i++)
            {
                unsigned char dec = static_cast<unsigned char>(str[i]);
                dec -= lineEdit_first_key.toInt();
                dec -= lineEdit_second_key.toInt();
                WriteStream << dec;
            }

            WriteStream.close();//закрываем поток на запись
        }

    }

}

