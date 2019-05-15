/********************************************************************************
** Form generated from reading UI file 'blockexplorer.ui'
**
** Created by: Qt User Interface Compiler version 5.12.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BLOCKEXPLORER_H
#define UI_BLOCKEXPLORER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BlockExplorer
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *back;
    QPushButton *forward;
    QPushButton *lastBlock;
    QLineEdit *searchBox;
    QPushButton *pushSearch;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QLabel *content;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *BlockExplorer)
    {
        if (BlockExplorer->objectName().isEmpty())
            BlockExplorer->setObjectName(QString::fromUtf8("BlockExplorer"));
        BlockExplorer->resize(1225, 600);
        centralwidget = new QWidget(BlockExplorer);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        back = new QPushButton(centralwidget);
        back->setObjectName(QString::fromUtf8("back"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/back"), QSize(), QIcon::Normal, QIcon::Off);
        back->setIcon(icon);
        back->setFlat(false);

        horizontalLayout->addWidget(back);

        forward = new QPushButton(centralwidget);
        forward->setObjectName(QString::fromUtf8("forward"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/forward"), QSize(), QIcon::Normal, QIcon::Off);
        forward->setIcon(icon1);

        horizontalLayout->addWidget(forward);

        lastBlock = new QPushButton(centralwidget);
        lastBlock->setObjectName(QString::fromUtf8("lastBlock"));

        horizontalLayout->addWidget(lastBlock);

        searchBox = new QLineEdit(centralwidget);
        searchBox->setObjectName(QString::fromUtf8("searchBox"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(5);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchBox->sizePolicy().hasHeightForWidth());
        searchBox->setSizePolicy(sizePolicy);
        searchBox->setMinimumSize(QSize(0, 0));

        horizontalLayout->addWidget(searchBox);

        pushSearch = new QPushButton(centralwidget);
        pushSearch->setObjectName(QString::fromUtf8("pushSearch"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushSearch->sizePolicy().hasHeightForWidth());
        pushSearch->setSizePolicy(sizePolicy1);
        pushSearch->setMinimumSize(QSize(0, 0));
        pushSearch->setMaximumSize(QSize(16777215, 16777215));
        pushSearch->setAutoDefault(false);
        pushSearch->setFlat(false);

        horizontalLayout->addWidget(pushSearch);


        verticalLayout->addLayout(horizontalLayout);

        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1205, 503));
        horizontalLayout_2 = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        content = new QLabel(scrollAreaWidgetContents);
        content->setObjectName(QString::fromUtf8("content"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(content->sizePolicy().hasHeightForWidth());
        content->setSizePolicy(sizePolicy2);
        content->setTextFormat(Qt::RichText);
        content->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        content->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_2->addWidget(content);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        BlockExplorer->setCentralWidget(centralwidget);
#ifndef Q_OS_MAC
        menubar = new QMenuBar(BlockExplorer);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1225, 22));
        BlockExplorer->setMenuBar(menubar);
#endif
        statusbar = new QStatusBar(BlockExplorer);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        BlockExplorer->setStatusBar(statusbar);

        retranslateUi(BlockExplorer);

        pushSearch->setDefault(false);


        QMetaObject::connectSlotsByName(BlockExplorer);
    } // setupUi

    void retranslateUi(QMainWindow *BlockExplorer)
    {
        BlockExplorer->setWindowTitle(QApplication::translate("BlockExplorer", "Blockchain Explorer", nullptr));
        back->setText(QApplication::translate("BlockExplorer", "back", nullptr));
        forward->setText(QApplication::translate("BlockExplorer", "forward", nullptr));
        lastBlock->setText(QApplication::translate("BlockExplorer", "Last Block", nullptr));
        searchBox->setInputMask(QString());
        searchBox->setText(QString());
        searchBox->setPlaceholderText(QApplication::translate("BlockExplorer", "Address / Block / Transaction", nullptr));
        pushSearch->setText(QApplication::translate("BlockExplorer", "Search", nullptr));
        content->setText(QApplication::translate("BlockExplorer", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BlockExplorer: public Ui_BlockExplorer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BLOCKEXPLORER_H
