#include <QtGui>
 
class Widget : public QWidget
{
public:
  Widget(QWidget *parent = 0)
    : QWidget(parent)
  {}
 
  void paintEvent(QPaintEvent *)
  {
    QPainter p(this);
#if 1
    QTextDocument document;
    document.setHtml("<br>T<br>e<br>s<br>t<br>");
    document.drawContents(&p);
#else
    drawRotatedText(&p, 90, width() / 2, height() / 2, "The vertical text");
#endif
  }
 
  void drawRotatedText(QPainter *painter, float degrees, int x, int y, const QString &text)
  {
    painter->save();
    painter->translate(x, y);
    painter->rotate(degrees);
    painter->drawText(0, 0, text);
    painter->restore();
  }
};

int main(int argc, char **argv)
{
  QApplication a(argc, argv);
  Widget w;
  w.resize(200,200);
  QString string = "t\ne\ns\nt";
  QLabel label;
  label.setText(string);
  label.show();
  w.show();
  return a.exec();
}
  
