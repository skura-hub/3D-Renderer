#include "mycontrollerobject.h"

MyControllerObject::MyControllerObject(QWidget *parent) : QWidget(parent)
{
    ready = false;
    mainLayout = new QVBoxLayout(this);
    buttonLayout = new QHBoxLayout();
    addTexture = new QPushButton("Dodaj Teksture", this);
    deleteTexture = new QPushButton("Wyczyść teksture", this);
    loadFile = new QFileDialog(this);

    createFormGroup();
    buttonLayout->addWidget(addTexture);
    buttonLayout->addWidget(deleteTexture);

    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);
    this->setLayout(mainLayout);
    connect(deleteTexture,
            SIGNAL(clicked()),
            this,
            SLOT(slot_texture_deleted()) );
    connect(addTexture,
                SIGNAL(clicked()),
                loadFile,
                SLOT(open())    );
    connect(loadFile,
            SIGNAL(fileSelected(const QString&)),
            this,
            SLOT(slot_texture_file_added(const QString &)));

    ready = true;
}

void MyControllerObject::slot_setCenter(const QVector3D &vector)
{
    ready = false;
    for(int i = 0; i < 3; i++){
        lineEdit[i]->setText(validator->locale().toString(vector[i]));
    }
    ready = true;
}

void MyControllerObject::slot_changed_lineEdits(const QString &line)
{
    int id = sender()->property("type").toInt();
    float value = validator->locale().toFloat(line);
    if(id == 0){
        vector.setX(value);
    }
    else if(id == 1){
        vector.setY(value);
    }
    else if(id == 2){
        vector.setZ(value);
    }
    if(ready) emit(sig_vector_changed(vector));
}

void MyControllerObject::slot_texture_file_added(const QString &path)
{
    QImage texture(path);
    if(texture.isNull()){
           QMessageBox msgBox;
           msgBox.setText("Plik nie jest właściwy");
           msgBox.exec();
       }
    else {
        emit(sig_texture_added(texture));
    }
}

void MyControllerObject::slot_texture_deleted()
{
    emit(sig_texture_deleted());
}

void MyControllerObject::createFormGroup()
{
    groupBox = new QGroupBox("Centrum");

    validator = new QDoubleValidator(this);
    layout = new QFormLayout(groupBox);

    //layout->addWidget(new QLabel("Centrum:", this));
    QString s[3];
    s[0] = "x";
    s[1] = "y";
    s[2] = "z";
    for(int i = 0; i < 3; i++){
        lineEdit[i] = new QLineEdit(this);
        lineEdit[i]->setValidator(validator);
        lineEdit[i]->setProperty("type", i);
        connect(lineEdit[i],
                SIGNAL(textChanged(const QString &)),
                this,
                SLOT(slot_changed_lineEdits(const QString &)));
        layout->addRow(new QLabel(s[i], this), lineEdit[i]);
    }
    groupBox->setLayout(layout);
}
