#ifndef DIALOG1_H
#define DIALOG1_H

#include <QDialog>

namespace Ui {
class Dialog1;
}

class Dialog1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog1(QWidget *parent = nullptr);
    ~Dialog1();

private slots:
    void on_assure_clicked();


signals:
    void assure1(bool flagsuccess);
private:
    Ui::Dialog1 *ui;
};

#endif // DIALOG1_H
