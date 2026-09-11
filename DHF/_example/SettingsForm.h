//---------------------------------------------------------------------------

#ifndef SettingsFormH
#define SettingsFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Mask.hpp>
#include "CSPIN.h"
//---------------------------------------------------------------------------
//Тут определяем #include

//---------------------------------------------------------------------------
class TFormSettings : public TForm
{
__published:	// IDE-managed Components

    //-----------------------------------------------------------------------
    
    //Объект "TImage *ImageModule"
    //должен присутствовать всегда
    //В ImageModule вставляем иконку модуля (16 на 16)!!!

    TImage *ImageModule;
    TLabel *Label1;
    TEdit *StrEdit;
    TCheckBox *BoolCheckBox;
    TListView *ListView1;
    TCSpinEdit *CSpinEdit1;

    //-----------------------------------------------------------------------
    
private:	// User declarations
public:		// User declarations
    __fastcall TFormSettings(TComponent* Owner);
    __fastcall ~TFormSettings();

protected:
    //-----------------------------------------------------------------------
    //Тут определяем свои локальные переменные и методы

    //-----------------------------------------------------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSettings *FormSettings;
//---------------------------------------------------------------------------
#endif
