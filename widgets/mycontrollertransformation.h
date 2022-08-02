#ifndef MYCONTROLLERTRANSFORMATION_H
#define MYCONTROLLERTRANSFORMATION_H

#include <QWidget>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QWidgetList>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QValidator>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLocale>


#include"matrixtransformation.h"

class MyControllerTransformation : public QWidget
{
    Q_OBJECT
private:
    enum class ButtonType{
        typ = 0, x, y, z, alfa, axis, last = axis
    };
    bool ready;
    QVBoxLayout *mainLayout;

    QGroupBox * groupBox;
    QFormLayout *layout;
    QLabel * labels [static_cast<int>(ButtonType::last) + 1];
    QLineEdit * lineEdit[4];
    QComboBox * axisChooser;
    QDoubleValidator * validator;
    QComboBox * comboBoxType;

    QVector3D vector;
    Transformation::Axis axis;
    float alfa;
    Transformation::Type type;

    void createFormGroup();

public:
    explicit MyControllerTransformation(QWidget *parent = nullptr);
signals:
    void sig_transformationChanged(const Transformation & transformation);
private slots:
    void slot_actionChanged();
    void slot_typeChanged(int i);
    void slot_changed_lineEdits(const QString & line);
    void slot_changed_axis(int i);
    void slot_lineEdit_vector_show();
    void slot_lineEdit_alfa_show();
    void slot_comboBox_axis_show();
    void slot_lineEdit_vector_hide();
    void slot_lineEdit_alfa_hide();
    void slot_comboBox_axis_hide();
};

#endif // MYCONTROLLERTRANSFORMATION_H
