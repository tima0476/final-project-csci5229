//
//  Qt Mountain Meadow Simulation
//

#include <QApplication>
#include "gl-widget.h"

//
//  Main function
//
int main(int argc, char *argv[])
{
   //  Create the application
   QApplication app(argc,argv);
   //  Create and show Viewer widget
   GLWidget gl_widget;
   gl_widget.show();
   //  Main loop for application
   return app.exec();
}
