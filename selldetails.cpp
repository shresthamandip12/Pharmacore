#include "selldetails.h"
#include "ui_selldetails.h"
#include "dashboard.h"
#include "stocks.h"
#include<QMessageBox>
#include<QDate>
#include<QFile>
#include<QTextStream>
#include<QSqlField>

selldetails::selldetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selldetails)
{
    ui->setupUi(this);


}

selldetails::~selldetails()
{
    delete ui;
}

void selldetails::on_pushButton_back_clicked()
{
    hide();
    Dashboard d;
    d.setModal(true);
    d.exec();
}





void selldetails::on_pushButton_pressed()
{
    Stocks s ;
     QString val = ui->lineEdit_search->text();
     if(!s.connOpen()){
         qDebug()<<"failed to connect to database";
         return;
     }

     s.connOpen();
     QSqlQuery qry;
     qry.prepare("select *  from table_stock where did like'%"+val+"%'or dcmpny like'%"+val+"%' ");


     if(qry.exec()) {
         while(qry.next())
         {
             ui->lineEdit_drugid->setText(qry.value(0).toString());
             ui->lineEdit_name->setText(qry.value(2).toString());
            if(qry.value(3).toInt()<=0)
            {
                QMessageBox::critical(this,tr("Quantity"),tr("Out of Stock"));
            }else {
                 ui->label_5->setText(qry.value(3).toString());
              }

             ui->lineEdit_price->setText(qry.value(4).toString());
         }
         s.connClose();

     }
     else {
       QMessageBox::critical(this,tr("Quantity"),tr("Out of Stock"));
     }
}

void selldetails::on_pushButton_add_clicked()
{
    Stocks s;

    QString billno,cname,drname,id,sqnty,sprice,dis;

    id = ui->lineEdit_drugid->text();
    billno= ui->lineEdit_billno->text();
    cname = ui->lineEdit_customername->text();
    drname = ui->lineEdit_name->text();
     sqnty = ui->lineEdit_quantity->text();
     dis = ui->lineEdit_discount->text();
    sprice = ui->lineEdit_price->text();


    if(!s.connOpen()){
        qDebug()<<"connection failed";
    }
    s.connOpen();
    QSqlQuery qrys,qry,qr,qry1,qry2;
    qrys.prepare("insert into table_billrecord(dname,qnty,discount,price) values('"+drname+"','"+sqnty+"','"+dis+"','"+sprice+"')");
    qry1.prepare("insert into table_salerecord(bill_no,dname,qnty,discount,price) values('"+billno+"','"+drname+"','"+sqnty+"','"+dis+"','"+sprice+"')");
    qry.prepare("update table_billrecord set total = (qnty*price)-(qnty*price*discount*0.01) ");
    qry2.prepare("update table_salerecord set total = (qnty*price)-(qnty*price*discount*0.01) ");
    qr.prepare("update table_stock set dqnty = dqnty - (select qnty from table_billrecord) where did = '"+id+"' ");
    if(qrys.exec()){
        QMessageBox::critical(this,tr("Save"),tr("Saved"));
        qry1.exec();
        qry2.exec();
        qry.exec();
        qr.exec();
        qry2.exec();

        ui->lineEdit_name->clear();
        ui->lineEdit_quantity->clear();
        ui->lineEdit_discount->clear();
        ui->lineEdit_price->clear();
        s.connClose();
    }else{
        QMessageBox::critical(this,tr("error::"),qrys.lastError().text());
    }


}

void selldetails::on_pushButton_refresh_clicked()
{
    Stocks s;
    QSqlQueryModel *m = new QSqlQueryModel();
    s.connOpen();
    QSqlQuery *qry = new QSqlQuery(s.mydb2);
    qry->prepare("select dname as Drug,qnty as Quantity,price as Price,discount as Discount,total as Total from table_billrecord");
    qry->exec();
    m->setQuery(*qry);
    ui->tableView->setModel(m);
    s.connClose();
    qDebug() <<( m->rowCount());
}

void selldetails::on_pushButton_reset_clicked()
{
   Stocks s;
    if(!s.connOpen()){
                qDebug()<<"connection failed";
            }
            s.connOpen();
            QSqlQuery qrys;
            qrys.prepare("delete from table_billrecord ");
            if(qrys.exec()){
                QMessageBox::information(this,tr("Reset"),tr("Reseted"));
               s. connClose();
            }else{
                QMessageBox::critical(this,tr("error::"),qrys.lastError().text());
            }
}

void selldetails::on_pushButton_totalamnt_clicked()
{
    Stocks s;
    QSqlQueryModel *model = new QSqlQueryModel();
    s.connOpen();
    model->setQuery("SELECT total from table_billrecord");
    ui->tableView->setModel(model);
    int result = 0;
        const int column = 0;
        for (int row = 0; row < model->rowCount(); ++row) {
            result += model->data(model->index(row, column)).toInt();
        }
        ui->lineEdit_total->setText( QString::number(result) );
        s.connClose();
}


void selldetails::on_pushButton_saveinfo_clicked()
{
    Stocks s;
    QString billno,cname,sdate,stotal;
    billno= ui->lineEdit_billno->text();
    cname = ui->lineEdit_customername->text();
     sdate = ui->dateEdit->date().toString("dd/MM/yyyy");
     stotal= ui->lineEdit_total->text();
        if(!s.connOpen()){
        qDebug()<<"connection failed";
    }
    s.connOpen();
    QSqlQuery qrys;
    qrys.prepare("insert into table_sellcomplete(bill_no,customer_name,date,total_amnt) values('"+billno+"','"+cname+"','"+sdate+"','"+stotal+"')");
    if(qrys.exec()){
        QMessageBox::critical(this,tr("Save"),tr("Saved"));
        foreach(QLineEdit *widget, this->findChildren<QLineEdit*>()) {
            widget->clear();
        }
        s.connClose();
    }else{
        QMessageBox::critical(this,tr("error::"),qrys.lastError().text());
    }
}
QString escCSV(QString unexc)
{
    if (!unexc.contains(QLatin1Char(',')))
        return unexc;
    return '\"' + unexc.replace(QLatin1Char('\"'), QStringLiteral("\"\"")) + '\"';
}

bool selldetails:: queryToCsv()
{

    Stocks conn;
    QSqlQueryModel * modal = new QSqlQueryModel();

    conn.connOpen();
    QSqlQuery* qry= new QSqlQuery(conn.mydb2);

    qry->prepare("Select * from table_billrecord");

    qry->exec();
    modal->setQuery(*qry);
    QFile csvFile("D:/PHARMACY/bill/bill.csv");
    if (!csvFile.open(QFile::WriteOnly | QFile::Text)){
        qDebug("Failed to open csv file");
    }
    QSqlRecord r;
    QTextStream writer(&csvFile);
    QStringList t;
    t  << "id" << "dname"<<"qnty"<< "discount"<<"price"<<"total";
    writer << t.join(',')+"\n";
    t.clear();
    for (int i = 0; i < modal->rowCount(); i++) {
        r = modal->record(i);
        t << r.value(0).toString() << r.value(1).toString() << r.value(2).toString()<< r.value(3).toString()<< r.value(4).toString() << r.value(5).toString()<< r.value(6).toString();
        writer << t.join(',') + "\n";
        t.clear();
    }

    csvFile.close();
    return true;
 }

void selldetails::on_pushButton_print_clicked()
{
            if(!queryToCsv())
            {

                QMessageBox::information(this,tr("Error!"),tr("Database File Not Exported."));
            }
            else {
                QMessageBox::information(this,tr(""),tr("Database File Exported."));
            }

}
