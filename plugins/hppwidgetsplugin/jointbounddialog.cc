#include "jointbounddialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <QDebug>

JointBoundDialog::JointBoundDialog(QString name, std::size_t nbDof, QWidget *parent) :
  QDialog(parent, Qt::Dialog)
{
  QVBoxLayout* vl = new QVBoxLayout (this);
  for (std::size_t i =0; i < nbDof; ++i) {
      QWidget* w = new QWidget (this);
      QHBoxLayout* hl = new QHBoxLayout (w);
      Line l (name + " " + QString::number (i), w);
      l.addToLayout(hl);
      lines_.append(l);
      vl->addWidget(w);
    }
  QDialogButtonBox *buttonBox = new QDialogButtonBox();
//  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
//  buttonBox->setGeometry(QRect(30, 240, 341, 32));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  vl->addWidget(buttonBox);

  QObject::connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

}

void JointBoundDialog::setBounds(const hpp::corbaserver::jointBoundSeq &bounds)
{
  if (bounds.length () != lines_.length() * 2) {
      qDebug () << "Wrong bounds dimensions";
      return;
    }
  std::size_t i = 0;
  foreach (const Line& l, lines_) {
      l.min->setValue(bounds[i]); i++;
      l.max->setValue(bounds[i]); i++;
    }
}

void JointBoundDialog::getBounds(hpp::corbaserver::jointBoundSeq_out bounds) const
{
  bounds->length (lines_.length() * 2);
  std::size_t i = 0;
  foreach (const Line& l, lines_) {
      bounds[i] = l.min->value(); i++;
      bounds[i] = l.max->value(); i++;
    }
}

JointBoundDialog::~JointBoundDialog()
{
}


JointBoundDialog::Line::Line(const QString &name, QWidget *parent)
  : label (new QLabel (name,parent))
  , min (new QDoubleSpinBox (parent))
  , max (new QDoubleSpinBox (parent))
{
}

void JointBoundDialog::Line::addToLayout(QLayout *l)
{
  l->addWidget(label);
  l->addWidget(min);
  l->addWidget(max);
}
