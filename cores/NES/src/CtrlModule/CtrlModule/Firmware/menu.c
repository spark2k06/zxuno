#include "osd.h"
#include "menu.h"
#include "keyboard.h"
#include "host.h"

int joykeys1;
int joykeys2;
int keys_p1[8] = { 0x1d, 0x1b, 0x1c, 0x23, 0x2b, 0x2d, 0x2e, 0x16}; // WSAD -> FR (Default) (5,1)
int keys_p2[8] = { 0x43, 0x42, 0x3b, 0x4b, 0x33, 0x35, 0x36, 0x1e}; // IKJL -> HY (Default) (6,2)

static struct menu_entry *menu;
static int menu_visible=0;
int menu_toggle_bits=0;
static int menurows;
static int currentrow;
static struct hotkey *hotkeys;

struct menu_entry *Menu_Get()
{
	return(menu);
}

void Menu_Show()
{
	OSD_Show(menu_visible=1);
}

void Menu_Hide()
{
	// Wait for key releases before hiding the menu, to avoid stray keyup messages reaching the host core.
	while(TestKey(KEY_ESC) || TestKey(KEY_ENTER) || TestKey(keys_p1[4]))
		HandlePS2RawCodes();
	OSD_Show(menu_visible=0);
}


static void DrawSlider(struct menu_entry *m)
{
	int i;
	for(i=0;i<=MENU_SLIDER_MAX(m);++i) // One extra character to leave a space before the label
	{
		OSD_Putchar(i<MENU_SLIDER_VALUE(m) ? 0x07 : 0x20);
	}
}


void Menu_Draw()
{
	struct menu_entry *m=menu;
	OSD_Clear();
	menurows=0;
	while(m->type!=MENU_ENTRY_NULL)
	{
		int i;
		char **labels;
		OSD_SetX(2);
		OSD_SetY(menurows);
		switch(m->type)
		{
			case MENU_ENTRY_CYCLE:
				i=MENU_CYCLE_VALUE(m);	// Access the first byte
				labels=(char**)m->label;
				OSD_Puts("\x16 ");
				OSD_Puts(labels[i]);
				break;
			case MENU_ENTRY_SLIDER:
				DrawSlider(m);
				OSD_Puts(m->label);
				break;
			case MENU_ENTRY_TOGGLE:
				if((menu_toggle_bits>>MENU_ACTION_TOGGLE(m->action))&1)
					OSD_Puts("\x14 ");
				else
					OSD_Puts("\x15 ");
				// Fall through
			default:
				OSD_Puts(m->label);
				break;
		}
		++menurows;
		m++;
	}
}


void Menu_Set(struct menu_entry *head)
{
	menu=head;
	Menu_Draw();
	currentrow=menurows-1;
}


void Menu_SetHotKeys(struct hotkey *head)
{
	hotkeys=head;
}

int Menu_Run()
{
	int i;
	struct menu_entry *m=menu;
	struct hotkey *hk=hotkeys;

	if(TestKey(KEY_ESC)&2)
	{
		while(TestKey(KEY_ESC))
			HandlePS2RawCodes(); // Wait for KeyUp message before opening OSD, since this disables the keyboard for the MSX core.
		OSD_Show(menu_visible^=1);
	}

	joykeys1=0;
    joykeys2=0;
	
	int auxkey;
	
	if(TestKey(KEY_F4)&2) // Swapping players keys...
	{
		auxkey = keys_p2[0]; keys_p2[0] = keys_p1[0]; keys_p1[0] = auxkey;
		auxkey = keys_p2[1]; keys_p2[1] = keys_p1[1]; keys_p1[1] = auxkey;
		auxkey = keys_p2[2]; keys_p2[2] = keys_p1[2]; keys_p1[2] = auxkey;
		auxkey = keys_p2[3]; keys_p2[3] = keys_p1[3]; keys_p1[3] = auxkey;
		auxkey = keys_p2[4]; keys_p2[4] = keys_p1[4]; keys_p1[4] = auxkey;
		auxkey = keys_p2[5]; keys_p2[5] = keys_p1[5]; keys_p1[5] = auxkey;
        auxkey = keys_p2[6]; keys_p2[6] = keys_p1[6]; keys_p1[6] = auxkey;
        auxkey = keys_p2[7]; keys_p2[7] = keys_p1[7]; keys_p1[7] = auxkey;
	}		

	if(!menu_visible)	// Swallow any keystrokes that occur while the OSD is hidden...
	{

		// Player 1
		if(TestKey(keys_p1[0])) // Up
			joykeys1|=0x01;
		if(TestKey(keys_p1[1])) // Down
			joykeys1|=0x02;
		if(TestKey(keys_p1[2])) // Left
			joykeys1|=0x04;
		if(TestKey(keys_p1[3])) // Right
			joykeys1|=0x08;
		if(TestKey(keys_p1[4])) // B1
			joykeys1|=0x10;
		if(TestKey(keys_p1[5])) // B2
			joykeys1|=0x20;
		if(TestKey(keys_p1[6])) // Select
			joykeys1|=0x40;
		if(TestKey(keys_p1[7])) // Start
			joykeys1|=0x80;
			
		// Player 2
		if(TestKey(keys_p2[0])) // Up
			joykeys2|=0x01;
		if(TestKey(keys_p2[1])) // Down
			joykeys2|=0x02;
		if(TestKey(keys_p2[2])) // Left
			joykeys2|=0x04;
		if(TestKey(keys_p2[3])) // Right
			joykeys2|=0x08;
		if(TestKey(keys_p2[4])) // B1
			joykeys2|=0x10;
		if(TestKey(keys_p2[5])) // B2
			joykeys2|=0x20;
		if(TestKey(keys_p2[6])) // Select
			joykeys2|=0x40;
		if(TestKey(keys_p2[7])) // Start
			joykeys2|=0x80;
		
		HW_HOST(REG_HOST_JOYKEY1) = joykeys1;
        HW_HOST(REG_HOST_JOYKEY2) = joykeys2;

		//q       
		// master reset
		if ((TestKey(KEY_LCTRL) || TestKey(KEY_RCTRL)) && (TestKey(KEY_ALT) || TestKey(KEY_ALTGR)) && TestKey(KEY_BACKSP) ) 
		{
			masterReset();
		}
		//q

		TestKey(KEY_PAGEUP);
		TestKey(KEY_PAGEDOWN);

		return;
	}

	// master reset
	if ((TestKey(KEY_LCTRL) || TestKey(KEY_RCTRL)) && (TestKey(KEY_ALT) || TestKey(KEY_ALTGR)) && TestKey(KEY_BACKSP) ) 
	{
		masterReset();
	}

	if((TestKey(KEY_UPARROW)&2) || (TestKey(keys_p1[0])&2))
	{
		if(currentrow)
			--currentrow;
		else if((m+menurows)->action)
			MENU_ACTION_CALLBACK((m+menurows)->action)(ROW_LINEUP);
	}
	if((TestKey(KEY_DOWNARROW)&2) || (TestKey(keys_p1[1])&2))
	{
		if(currentrow<(menurows-1))
			++currentrow;
		else if((m+menurows)->action)
			MENU_ACTION_CALLBACK((m+menurows)->action)(ROW_LINEDOWN);
	}

	if((TestKey(KEY_PAGEUP)&2) || (TestKey(keys_p1[2])&2))
	{
		if(currentrow)
			currentrow=0;
		else if((m+menurows)->action)
			MENU_ACTION_CALLBACK((m+menurows)->action)(ROW_PAGEUP);
	}

	if((TestKey(KEY_PAGEDOWN)&2) || (TestKey(keys_p1[3])&2))
	{
		if(currentrow<(menurows-1))
			currentrow=menurows-1;
		else if((m+menurows)->action)
			MENU_ACTION_CALLBACK((m+menurows)->action)(ROW_PAGEDOWN);
	}

	// Find the currently highlighted menu item
	i=currentrow;
	while(i)
	{
		++m;
		--i;
	}

	OSD_SetX(2);
	OSD_SetY(currentrow);

	if(TestKey(KEY_LEFTARROW)&2) // Decrease slider value
	{
		switch(m->type)
		{
			case MENU_ENTRY_SLIDER:
				if((--MENU_SLIDER_VALUE(m))&0x80) // <0?
					MENU_SLIDER_VALUE(m)=0;
				DrawSlider(m);
				break;
			default:
				break;
		}
	}

	if(TestKey(KEY_RIGHTARROW)&2) // Increase slider value
	{
		switch(m->type)
		{
			case MENU_ENTRY_SLIDER:
				if((++MENU_SLIDER_VALUE(m))>MENU_SLIDER_MAX(m))
					MENU_SLIDER_VALUE(m)=MENU_SLIDER_MAX(m);
				DrawSlider(m);
				break;
			default:
				break;
		}
	}


	if((TestKey(KEY_ENTER)&2) || (TestKey(keys_p1[4])&2))
	{
		struct menu_entry *m=menu;
		i=currentrow;
		while(i)
		{
			++m;
			--i;
		}
		switch(m->type)
		{
			case MENU_ENTRY_SUBMENU:
				Menu_Set(MENU_ACTION_SUBMENU(m->action));
				break;
			case MENU_ENTRY_CALLBACK:
				MENU_ACTION_CALLBACK(m->action)(currentrow);
				break;
			case MENU_ENTRY_TOGGLE:
				i=1<<MENU_ACTION_TOGGLE(m->action);
				menu_toggle_bits^=i;
				Menu_Draw();
				break;
			case MENU_ENTRY_CYCLE:
				i=MENU_CYCLE_VALUE(m)+1;
				if(i>=MENU_CYCLE_COUNT(m))
					i=0;
				MENU_CYCLE_VALUE(m)=i;
				Menu_Draw();
				break;
			default:
				break;

		}
	}

	while(hk && hk->key)
	{
		if(TestKey(hk->key)&1)	// Currently pressed?
			hk->callback(currentrow);
		++hk;
	}

	for(i=0;i<OSD_ROWS-1;++i)
	{
		OSD_SetX(0);
		OSD_SetY(i);
		OSD_Putchar(i==currentrow ? (i==menurows-1 ? 17 : 16) : 32);
	}

	return(menu_visible);
}
