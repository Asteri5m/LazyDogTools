#include "LazyDogTools.h"
#include "ui_lazydogtools.h"

LazyDogTools::LazyDogTools(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LazyDogTools)
{
    ui->setupUi(this);
}

LazyDogTools::~LazyDogTools()
{
    delete ui;
}
