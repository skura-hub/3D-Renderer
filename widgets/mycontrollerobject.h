#ifndef MYCONTROLLEROBJECT_H
#define MYCONTROLLEROBJECT_H

#include <QObject>
#include <QWidget>
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
#include <QVector3D>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>

class MyControllerObject : public QWidget
{
    Q_OBJECT
private:
    bool ready;
    QVBoxLayout *mainLayout;
    QGroupBox * groupBox;
    QFormLayout *layout;
    QHBoxLayout * buttonLayout;

    QPushButton * addTexture;
    QPushButton * deleteTexture;
    QFileDialog * loadFile;

    QLineEdit * lineEdit[3];
    QDoubleValidator * validator;
    QVector3D vector;

public:
    explicit MyControllerObject(QWidget *parent = nullptr);

signals:
    void sig_texture_added(const QImage & texture);
    void sig_texture_deleted();
    void sig_vector_changed(const QVector3D & vector);
public slots:
    void slot_setCenter(const QVector3D & vector);
private slots:
    void slot_changed_lineEdits(const QString & line);
    void slot_texture_file_added(const QString & path);
    void slot_texture_deleted();

private:
    void createFormGroup();
};

#endif // MYCONTROLLEROBJECT_H
