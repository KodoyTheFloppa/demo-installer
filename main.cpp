#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QProcess>
#include <QPushButton>
#include <QStackedLayout>
#include <QTemporaryFile>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setFixedSize(400, 200);
        QWidget *central = new QWidget;
        setCentralWidget(central);

        frame = new QFrame;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->addWidget(frame);

        btnBack = new QPushButton(QString::fromUtf8("Назад"));
        btnNext = new QPushButton(QString::fromUtf8("Далее"));
        btnBack->hide();

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(btnBack);
        buttonLayout->addStretch();
        buttonLayout->addWidget(btnNext);
        mainLayout->addLayout(buttonLayout);

        stacked = new QStackedLayout(frame);

        QWidget *page1 = new QWidget;
        QVBoxLayout *v1 = new QVBoxLayout(page1);
        QLabel *label = new QLabel(QString::fromUtf8("Пуненко"));
        label->setAlignment(Qt::AlignCenter);
        v1->addWidget(label);

        QWidget *page2 = new QWidget;
        QVBoxLayout *v2 = new QVBoxLayout(page2);
        combo = new QComboBox;
        packageResources.insert("PostgreSQL", ":/postgresql.deb");
        packageResources.insert("Wine", ":/wine.deb");
        combo->addItems(packageResources.keys());
        v2->addWidget(combo);

        stacked->addWidget(page1);
        stacked->addWidget(page2);

        connect(btnNext, &QPushButton::clicked, this, &MainWindow::onNext);
        connect(btnBack, &QPushButton::clicked, this, &MainWindow::onBack);
    }

   private slots:
    void onNext() {
        if (stacked->currentIndex() == 0) {
            stacked->setCurrentIndex(1);
            btnNext->setText(QString::fromUtf8("Установить"));
            btnBack->show();
        } else {
            QString sel = combo->currentText();
            QString resPath = packageResources.value(sel);
            QFile res(resPath);
            QString packagePath = QDir::tempPath() + "/package.deb";
            QFile::remove(packagePath);
            QFile::copy(resPath, packagePath);

            QProcess *process = new QProcess;
            process->setProgram("apt");
            process->setArguments(QStringList()
                                  << "install" << "-y" << packagePath);

            process->setProcessChannelMode(QProcess::MergedChannels);

            connect(process, &QProcess::started,
                    []() { qDebug() << "Process started"; });
            connect(process, &QProcess::errorOccurred,
                    [process](QProcess::ProcessError err) {
                        qDebug() << "Process failed with error:" << err << ","
                                 << process->errorString();
                    });

            connect(process, &QProcess::readyRead, [process]() {
                QByteArray out = process->readAll();
                qDebug() << out;
            });

            process->start();
        }
    }

    void onBack() {
        stacked->setCurrentIndex(0);
        btnNext->setText(QString::fromUtf8("Далее"));
        btnBack->hide();
    }

   private:
    QFrame *frame;
    QStackedLayout *stacked;
    QPushButton *btnNext;
    QPushButton *btnBack;
    QComboBox *combo;
    QMap<QString, QString> packageResources;
};

#include "main.moc"

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
