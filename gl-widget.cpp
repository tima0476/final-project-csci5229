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
   layout->setColumnStretch(0,100);       // determines how much of the available space the column will get over and 
                                          // above its necessary minimum.
   layout->setColumnMinimumWidth(0,800);
   layout->setRowMinimumHeight(0,800);
   layout->setRowStretch(0,100);
   layout->setContentsMargins(1, 1, 1, 1);


   // QGridLayout also includes two margin widths: the contents margin and the spacing(). The contents margin is the
   // width of the reserved space along each of the QGridLayout's four sides. The spacing() is the width of the
   // automatically allocated spacing between neighboring boxes.

   // The default contents margin values are provided by the style. The default value Qt styles specify is 9 for child
   // widgets and 11 for windows. The spacing defaults to the same as the margin width for a top-level layout, or to the
   // same as the parent layout.


   // void QGridLayout::addLayout(QLayout *layout, int row, int column, int rowSpan, int columnSpan, Qt::Alignment
   //  alignment = Qt::Alignment()) 
   
   // Lorenz widget
   layout->addWidget(main_window,0,0,1,1);


   //  Overall layout
   setLayout(layout);
}
