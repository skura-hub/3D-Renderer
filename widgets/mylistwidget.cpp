#include "mylistwidget.h"

MyListWidget::MyListWidget(QWidget *parent, bool dragAndDrop) : QWidget(parent)
{
    test = 0;
    vbox = new QVBoxLayout(this);
    vbox->setSpacing(10);

    hbox = new QHBoxLayout();

    lw = new QListWidget(this);

    if(dragAndDrop) setDragAndDropEnabled();
    add = new QPushButton("Add", this);
    remove = new QPushButton("Remove", this);

    hbox->setSpacing(3);
    hbox->addStretch(1);
    hbox->addWidget(add);
    hbox->addWidget(remove);
    hbox->addStretch(1);

    vbox->addLayout(hbox);
    vbox->addSpacing(15);
    vbox->addWidget(lw);


    connect(add, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(remove, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(lw->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(rowsMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(lw, SIGNAL(currentRowChanged(int)), this, SLOT(currentRowChanged(int)));
    setLayout(vbox);
}

void MyListWidget::removeItem(int id)
{
    QListWidgetItem *item = lw->takeItem(id);
    delete item;
}

void MyListWidget::addItem()
{
    lw->addItem(QString::number(test++));
    int r = size() - 1;

    emit(sig_itemAdded());
    lw->setCurrentRow(r);

}

void MyListWidget::removeItem()
{
    int r = lw->currentRow();
    if (r != -1) {
        removeItem(r);
        emit(sig_itemRemoved(r));
    }
}

void MyListWidget::rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    if(sourceStart < destinationRow) destinationRow--;
    emit(sig_itemMoved(sourceStart, destinationRow));
}

void MyListWidget::setDragAndDropEnabled()
{
    lw->setSelectionMode(QAbstractItemView::SingleSelection);
    lw->setDragEnabled(true);
    lw->setDefaultDropAction(Qt::MoveAction);
    lw->setAcceptDrops(true);
    lw->setDropIndicatorShown(true);
}

void MyListWidget::slot_setName(const QString &newName)
{
    QListWidgetItem *curitem = lw->currentItem();
    int r = lw->row(curitem);
    QListWidgetItem *item = lw->takeItem(r);
    delete item;
    lw->insertItem(r,newName);
    lw->setCurrentRow(r);
}
