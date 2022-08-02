#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>

class MyListWidget : public QWidget
{
    Q_OBJECT
private:
    QListWidget *lw;
    QPushButton *add;
    QPushButton *remove;

    QVBoxLayout *vbox;
    QHBoxLayout *hbox;
    int test;

public:
    explicit MyListWidget(QWidget *parent = nullptr, bool dragAndDrop = false);
    void setCurrentRow(int row){
        if(lw->currentRow() == row) emit(currentRowChanged(row));
        else lw->setCurrentRow(row);
    }
    void removeItem(int id);
    int size() const
        {return lw->count();}
private:
    void setDragAndDropEnabled();

signals:
    void sig_currentRowChanged(int currentRow);
    void sig_itemMoved(int oldId, int newId);
    void sig_itemAdded();
    void sig_itemRemoved(int id);

private slots:
    void addItem();
    void rowsMoved(const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    void currentRowChanged(int id)
        {if(id != -1) emit(sig_currentRowChanged(id));}
    void removeItem();

public slots:
    void slot_setName(const QString & newName);



};

#endif // MYLISTWIDGET_H
