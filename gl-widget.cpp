//
//  GLWidget Widget
//

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include "gl-widget.h"
#include "main-window.h"

//
//  Constructor for GLWidget widget
//
GLWidget::GLWidget(QWidget* parent) :  QWidget(parent)
{
   //  Set window title
   setWindowTitle(tr("Timothy Mason - Mountain Meadow Simulation"));

   //  Create new MainWindow widget
   MainWindow* main_window = new MainWindow;

   //  Set layout of child widgets
   QGridLayout* layout = new QGridLayout;
   layout->setColumnStretch(0,100);
   layout->setColumnMinimumWidth(0,100);
   layout->setRowStretch(0,100);

   //  Lorenz widget
   layout->addWidget(main_window,0,0,5,1);


   //  Overall layout
   setLayout(layout);
}
