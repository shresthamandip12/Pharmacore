#ifndef USERINSTRUCTION_H
#define USERINSTRUCTION_H

#include <QDialog>

namespace Ui {
class userinstruction;
}

class userinstruction : public QDialog
{
    Q_OBJECT

public:
    explicit userinstruction(QWidget *parent = nullptr);
    ~userinstruction();

private slots:
    void on_pushButton_clicked();

private:
    Ui::userinstruction *ui;
};

#endif // USERINSTRUCTION_H
