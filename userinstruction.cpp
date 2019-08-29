#include "userinstruction.h"
#include "ui_userinstruction.h"
#include "dashboard.h"
userinstruction::userinstruction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userinstruction)
{
    ui->setupUi(this);
}

userinstruction::~userinstruction()
{
    delete ui;
}

void userinstruction::on_pushButton_clicked()
{
    hide();
    Dashboard s1;
    s1.setModal(true);
    s1.exec();
}
