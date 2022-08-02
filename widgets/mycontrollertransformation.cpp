#include "mycontrollertransformation.h"

void MyControllerTransformation::createFormGroup()
{
    //QString title = "Transformacja " + QString::number(id_transformation + 1);
    groupBox = new QGroupBox("Transformacja");
    labels[ static_cast<int>(ButtonType::typ) ] = new QLabel("Typ:", this);
    labels[ static_cast<int>(ButtonType::x) ] = new QLabel("x:", this);
    labels[ static_cast<int>(ButtonType::y) ] = new QLabel("y:", this);
    labels[ static_cast<int>(ButtonType::z) ] = new QLabel("z:", this);
    labels[ static_cast<int>(ButtonType::alfa) ] = new QLabel("Kąt:", this);
    labels[ static_cast<int>(ButtonType::axis) ] = new QLabel("Oś:", this);

    validator = new QDoubleValidator(this);
    comboBoxType = new QComboBox(this);
    axisChooser = new QComboBox(this);
    layout = new QFormLayout(groupBox);
    layout->addRow(labels[0], comboBoxType);
    int firstType = static_cast<int>(ButtonType::x);
    for(int i = 0; i < 4; i++){
        lineEdit[i] = new QLineEdit(this);
        lineEdit[i]->setValidator(validator);
        lineEdit[i]->setProperty("type", firstType + i);
        connect(lineEdit[i],
                SIGNAL(textChanged(const QString &)),
                this,
                SLOT(slot_changed_lineEdits(const QString &)));
    }
    connect(axisChooser,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(slot_changed_axis(int)));
    connect(comboBoxType,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(slot_typeChanged(int)));

    comboBoxType->insertItem(0, "Translacja");
    comboBoxType->insertItem(1, "Skalowanie");
    comboBoxType->insertItem(2, "Ścinanie");
    comboBoxType->insertItem(3, "Obracanie");
    comboBoxType->setProperty("type", static_cast<int>(ButtonType::typ));

    axisChooser->insertItem(0, "OX");
    axisChooser->insertItem(1, "OY");
    axisChooser->insertItem(2, "OZ");
    axisChooser->setProperty("type", static_cast<int>(ButtonType::axis));

    groupBox->setLayout(layout);
}

MyControllerTransformation::MyControllerTransformation(QWidget *parent) : QWidget(parent)
{
    ready = false;
    mainLayout = new QVBoxLayout(this);
    createFormGroup();
    mainLayout->addWidget(groupBox);
    this->setLayout(mainLayout);
    slot_actionChanged();
    ready = true;
}

void MyControllerTransformation::slot_actionChanged()
{
    if(!ready) return;
    Transformation transformation;
    if(type == Transformation::Type::translation){
        transformation.setTranslation(vector);
    }
    else if(type == Transformation::Type::scale){

        if(qFuzzyIsNull(vector.x()) || qFuzzyIsNull(vector.x()) || qFuzzyIsNull(vector.x())) return;
        transformation.setScale(vector);
    }
    else if(type == Transformation::Type::shear){
        transformation.setShear(vector, axis);
    }
    else{
        transformation.setRotation(alfa, axis);
    }
    emit(sig_transformationChanged(transformation));
}

void MyControllerTransformation::slot_typeChanged(int i)
{
    ready = false;
    type = static_cast<Transformation::Type>(i);
    if(type == Transformation::Type::rotation || type == Transformation::Type::shear){
        slot_comboBox_axis_show();
    }
    else{
        slot_comboBox_axis_hide();
    }
    if(type != Transformation::Type::rotation){
        slot_lineEdit_vector_show();
        slot_lineEdit_alfa_hide();
    }
    else{
        slot_lineEdit_vector_hide();
        slot_lineEdit_alfa_show();
    }
    ready = true;
}

void MyControllerTransformation::slot_changed_lineEdits(const QString &line)
{
    int id = sender()->property("type").toInt();
    ButtonType sender_type = static_cast<ButtonType>(id);
    float value = validator->locale().toFloat(line);
    if(sender_type == ButtonType::alfa){
        alfa = value;
    }
    else if(sender_type == ButtonType::x){
        vector.setX(value);
    }
    else if(sender_type == ButtonType::y){
        vector.setY(value);
    }
    else if(sender_type == ButtonType::z){
        vector.setZ(value);
    }
    slot_actionChanged();
}

void MyControllerTransformation::slot_changed_axis(int i)
{
    axis = static_cast<Transformation::Axis>(i);
    slot_actionChanged();
}

void MyControllerTransformation::slot_lineEdit_vector_show()
{
    int first = static_cast<int>(ButtonType::x);
    for(int i = 0; i < 3; i++){
        layout->insertRow(first + i, labels[first + i], lineEdit[i]);
        labels[first + i]->show();
        lineEdit[i]->show();
        int value;
        if(type==Transformation::Type::translation || type == Transformation::Type::shear) value = 0;
        else value = 1;
        lineEdit[i]->setText(QString::number(value));
    }
}

void MyControllerTransformation::slot_lineEdit_alfa_show()
{
    int id = static_cast<int>(ButtonType::alfa);
    layout->insertRow(id, labels[id], lineEdit[3]);
    labels[id]->show();
    lineEdit[3]->show();
    lineEdit[3]->setText(QString::number(0));

}

void MyControllerTransformation::slot_comboBox_axis_show()
{
    int id = static_cast<int>(ButtonType::axis);
    layout->insertRow(id, labels[id], axisChooser);
    labels[id]->show();
    axisChooser->show();
    axisChooser->setCurrentIndex(0);
}

void MyControllerTransformation::slot_lineEdit_vector_hide()
{
    int first = static_cast<int>(ButtonType::x);
    for(int i = 0; i < 3; i++){
        labels[first + i]->hide();
        lineEdit[i]->hide();
        layout->removeWidget(labels[first + i]);
        layout->removeWidget(lineEdit[i]);
    }
}

void MyControllerTransformation::slot_lineEdit_alfa_hide()
{
    int id = static_cast<int>(ButtonType::alfa);
    labels[id]->hide();
    lineEdit[3]->hide();
    layout->removeWidget(labels[id]);
    layout->removeWidget(lineEdit[3]);
}

void MyControllerTransformation::slot_comboBox_axis_hide()
{
    int id = static_cast<int>(ButtonType::axis);
    labels[id]->hide();
    axisChooser->hide();
    layout->removeWidget(labels[id]);
    layout->removeWidget(axisChooser);
}
