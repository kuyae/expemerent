/*
 * Terra Informatica Sciter Engine
 * http://terrainformatica.com/sciter
 *
 * Behaviors support (a.k.a windowless controls)
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 *
 * (C) 2003-2004, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

#ifndef __sciter_x_behavior_h__
#define __sciter_x_behavior_h__

/*!\file
\brief Behaiviors support (a.k.a windowless scriptable controls)
*/
#if defined(PLATFORM_WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
#endif


#include "sciter-x-dom.h"
#include "sciter-x-value.h"

#pragma pack(push,8)

  /** event groups.
       **/
  enum EVENT_GROUPS
  {
      HANDLE_INITIALIZATION = 0x0000, /** attached/detached */
      HANDLE_MOUSE = 0x0001,          /** mouse events */
      HANDLE_KEY = 0x0002,            /** key events */
      HANDLE_FOCUS = 0x0004,          /** focus events, if this flag is set it also means that element it attached to is focusable */
      HANDLE_SCROLL = 0x0008,         /** scroll events */
      HANDLE_TIMER = 0x0010,          /** timer event */
      HANDLE_SIZE = 0x0020,           /** size changed event */
      HANDLE_DRAW = 0x0040,           /** drawing request (event) */
      HANDLE_DATA_ARRIVED = 0x080,    /** requested data () has been delivered */
      HANDLE_BEHAVIOR_EVENT = 0x0100, /** secondary, synthetic events:
                                        BUTTON_CLICK, HYPERLINK_CLICK, etc.,
                                        a.k.a. notifications from intrinsic behaviors */
      HANDLE_METHOD_CALL = 0x0200,    /** behavior specific methods */
      HANDLE_SCRIPTING_METHOD_CALL = 0x0400, /** behavior specific methods */

      HANDLE_ALL = 0x07FF             /* all of them */
  };

/**Element callback function for all types of events. Similar to WndProc
 * \param tag \b LPVOID, tag assigned by HTMLayoutAttachElementProc function (like GWL_USERDATA)
 * \param he \b HELEMENT, this element handle (like HWND)
 * \param evtg \b UINT, group identifier of the event, value is one of EVENT_GROUPS
 * \param prms \b LPVOID, pointer to group specific parameters structure.
 * \return TRUE if event was handled, FALSE otherwise.
 **/

typedef  BOOL CALLBACK ElementEventProc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms );
typedef  ElementEventProc * LPElementEventProc;

  enum PHASE_MASK
  {
      BUBBLING = 0,      // bubbling (emersion) phase
      SINKING  = 0x8000,  // capture (immersion) phase, this flag is or'ed with EVENTS codes below
      HANDLED  = 0x10000
    // see: http://www.w3.org/TR/xml-events/Overview.html#s_intro
  };

  enum MOUSE_BUTTONS
  {
      MAIN_MOUSE_BUTTON = 1, //aka left button
      PROP_MOUSE_BUTTON = 2, //aka right button
      MIDDLE_MOUSE_BUTTON = 4,
  };

  enum KEYBOARD_STATES
  {
      CONTROL_KEY_PRESSED = 0x1,
      SHIFT_KEY_PRESSED = 0x2,
      ALT_KEY_PRESSED = 0x4
  };

// parameters of evtg == HANDLE_INITIALIZATION

  enum INITIALIZATION_EVENTS
  {
    BEHAVIOR_DETACH = 0,
    BEHAVIOR_ATTACH = 1
  };

  struct INITIALIZATION_PARAMS
  {
    UINT cmd; // INITIALIZATION_EVENTS
  };


// parameters of evtg == HANDLE_MOUSE

  enum MOUSE_EVENTS
  {
      MOUSE_ENTER = 0,
      MOUSE_LEAVE,
      MOUSE_MOVE,
      MOUSE_UP,
      MOUSE_DOWN,
      MOUSE_DCLICK,
      MOUSE_WHEEL,
      MOUSE_TICK, // mouse pressed ticks
      MOUSE_IDLE, // mouse stay idle for some time
  };

  struct MOUSE_PARAMS
  {
      UINT      cmd;          // MOUSE_EVENTS
      HELEMENT  target;       // target element
      POINT     pos;          // position of cursor, element relative
      POINT     pos_document; // position of cursor, document root relative
      UINT      button_state; // MOUSE_BUTTONS
      UINT      alt_state;    // KEYBOARD_STATES
      UINT      cursor_type;  // CURSOR_TYPE to set, see CURSOR_TYPE
      BOOL      is_on_icon;   // mouse is over icon (foreground-image, foreground-repeat:no-repeat)
  };

  enum CURSOR_TYPE
  {
      CURSOR_ARROW, //0
      CURSOR_IBEAM, //1
      CURSOR_WAIT,  //2
      CURSOR_CROSS, //3
      CURSOR_UPARROW,  //4
      CURSOR_SIZENWSE, //5
      CURSOR_SIZENESW, //6
      CURSOR_SIZEWE,   //7
      CURSOR_SIZENS,   //8
      CURSOR_SIZEALL,  //9
      CURSOR_NO,       //10
      CURSOR_APPSTARTING, //11
      CURSOR_HELP,        //12
      CURSOR_HAND,        //13
      CURSOR_DRAG_MOVE,   //14
      CURSOR_DRAG_COPY,   //15
  };


// parameters of evtg == HANDLE_KEY

  enum KEY_EVENTS
  {
      KEY_DOWN = 0,
      KEY_UP,
      KEY_CHAR
  };

  struct KEY_PARAMS
  {
      UINT      cmd;          // KEY_EVENTS
      HELEMENT  target;       // target element
      UINT      key_code;     // key scan code, or character unicode for KEY_CHAR
      UINT      alt_state;    // KEYBOARD_STATES
  };

// parameters of evtg == HANDLE_FOCUS

  enum FOCUS_EVENTS
  {
      FOCUS_LOST = 0,
      FOCUS_GOT = 1,
  };

  struct FOCUS_PARAMS
  {
      UINT      cmd;          // FOCUS_EVENTS
      HELEMENT  target;       // target element, for FOCUS_LOST it is a handle of new focus element
                                //    and for FOCUS_GOT it is a handle of old focus element, can be NULL
      BOOL      by_mouse_click; // TRUE if focus is being set by mouse click
      BOOL      cancel;         // in FOCUS_LOST phase setting this field to TRUE will cancel transfer focus from old element to the new one.
  };

// parameters of evtg == HANDLE_SCROLL

  enum SCROLL_EVENTS
  {
      SCROLL_HOME = 0,
      SCROLL_END,
      SCROLL_STEP_PLUS,
      SCROLL_STEP_MINUS,
      SCROLL_PAGE_PLUS,
      SCROLL_PAGE_MINUS,
      SCROLL_POS
  };

  struct SCROLL_PARAMS
  {
      UINT      cmd;          // SCROLL_EVENTS
      HELEMENT  target;       // target element
      INT       pos;          // scroll position if SCROLL_POS
      BOOL      vertical;     // TRUE if from vertical scrollbar
  };


  enum DRAW_EVENTS
  {
      DRAW_BACKGROUND = 0,
      DRAW_CONTENT = 1,
      DRAW_FOREGROUND = 2,
  };

  struct DRAW_PARAMS
  {
      UINT      cmd;          // DRAW_EVENTS
      HDC       hdc;          // hdc to paint on
      RECT      area;         // element area to paint,
      UINT      reserved;     //   for DRAW_BACKGROUND/DRAW_FOREGROUND - it is a border box
                              //   for DRAW_CONTENT - it is a content box
  };
  // Use ::GetTextColor(hdc) to get current text color of the element

  enum BEHAVIOR_EVENTS
  {
      BUTTON_CLICK = 0,              // click on button
      BUTTON_PRESS = 1,              // mouse down or key down in button
      BUTTON_STATE_CHANGED = 2,      // checkbox/radio/slider changed its state/value
      EDIT_VALUE_CHANGING = 3,       // before text change
      EDIT_VALUE_CHANGED = 4,        // after text change
      SELECT_SELECTION_CHANGED = 5,  // selection in <select> changed
      SELECT_STATE_CHANGED = 6,      // node in select expanded/collapsed, heTarget is the node

      POPUP_REQUEST   = 7,           // request to show popup just received,
                                     //     here DOM of popup element can be modifed.
      POPUP_READY     = 8,           // popup element has been measured and ready to be shown on screen,
                                     //     here you can use functions like ScrollToView.
      POPUP_DISMISSED = 9,           // popup element is closed,
                                     //     here DOM of popup element can be modifed again - e.g. some items can be removed
                                     //     to free memory.

      MENU_ITEM_ACTIVE = 0xA,        // menu item activated by mouse hover or by keyboard,
      MENU_ITEM_CLICK = 0xB,         // menu item click,
                                     //   BEHAVIOR_EVENT_PARAMS structure layout
                                     //   BEHAVIOR_EVENT_PARAMS.cmd - MENU_ITEM_CLICK/MENU_ITEM_ACTIVE
                                     //   BEHAVIOR_EVENT_PARAMS.heTarget - owner(anchor) of the menu
                                     //   BEHAVIOR_EVENT_PARAMS.he - the menu item, presumably <li> element
                                     //   BEHAVIOR_EVENT_PARAMS.reason - BY_MOUSE_CLICK | BY_KEY_CLICK


      CONTEXT_MENU_REQUEST = 0x10,   // "right-click", BEHAVIOR_EVENT_PARAMS::he is current popup menu HELEMENT being processed or NULL.
                                     // application can provide its own HELEMENT here (if it is NULL) or modify current menu element.

      VISIUAL_STATUS_CHANGED = 0x11, // broadcast notification, sent to all elements of some container being shown or hidden


      // "grey" event codes  - notfications from behaviors from this SDK
      HYPERLINK_CLICK = 0x80,        // hyperlink click
      TABLE_HEADER_CLICK,            // click on some cell in table header,
                                     //     target = the cell,
                                     //     reason = index of the cell (column number, 0..n)
      TABLE_ROW_CLICK,               // click on data row in the table, target is the row
                                     //     target = the row,
                                     //     reason = index of the row (fixed_rows..n)
      TABLE_ROW_DBL_CLICK,           // mouse dbl click on data row in the table, target is the row
                                     //     target = the row,
                                     //     reason = index of the row (fixed_rows..n)

      ELEMENT_COLLAPSED = 0x90,      // element was collapsed, so far only behavior:tabs is sending these two to the panels
      ELEMENT_EXPANDED,              // element was expanded,

      ACTIVATE_CHILD,                // activate (select) child,
                                     // used for example by accesskeys behaviors to send activation request, e.g. tab on behavior:tabs.

      DO_SWITCH_TAB = ACTIVATE_CHILD,// command to switch tab programmatically, handled by behavior:tabs
                                     // use it as HTMLayoutPostEvent(tabsElementOrItsChild, DO_SWITCH_TAB, tabElementToShow, 0);

      INIT_DATA_VIEW,                // request to virtual grid to initialize its view
      
      ROWS_DATA_REQUEST,             // request from virtual grid to data source behavior to fill data in the table
                                     // parameters passed throug DATA_ROWS_PARAMS structure.

      UI_STATE_CHANGED,              // ui state changed, observers shall update their visual states.
                                     // is sent for example by behavior:richtext when caret position/selection has changed.

      FORM_SUBMIT,                   // behavior:form detected submission event. BEHAVIOR_EVENT_PARAMS::data field contains data to be posted.
                                     // BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about 
                                     // to be submitted. You can modify the data or discard submission by returning TRUE from the handler.
      FORM_RESET,                    // behavior:form detected reset event (from button type=reset). BEHAVIOR_EVENT_PARAMS::data field contains data to be reset.
                                     // BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about 
                                     // to be rest. You can modify the data or discard reset by returning TRUE from the handler.
      FIRST_APPLICATION_EVENT_CODE = 0x100
      // all custom event codes shall be greater
      // than this number. All codes below this will be used
      // solely by application - HTMLayout will not intrepret it
      // and will do just dispatching.
      // To send event notifications with  these codes use
      // HTMLayoutSend/PostEvent API.

  };

  enum EVENT_REASON
  {
      BY_MOUSE_CLICK,
      BY_KEY_CLICK,
      SYNTHESIZED, // synthesized, programmatically generated.
  };

  enum EDIT_CHANGED_REASON
  {
      BY_INS_CHAR,  // single char insertion
      BY_INS_CHARS, // character range insertion, clipboard
      BY_DEL_CHAR,  // single char deletion
      BY_DEL_CHARS, // character range deletion (selection)
  };



  struct BEHAVIOR_EVENT_PARAMS
  {
      UINT     cmd;        // BEHAVIOR_EVENTS
      HELEMENT heTarget;   // target element handler, in MENU_ITEM_CLICK this is owner element that caused this menu - e.g. context menu owner
                           // In scripting this field named as Event.owner
      HELEMENT he;         // source element e.g. in SELECTION_CHANGED it is new selected <option>, in MENU_ITEM_CLICK it is menu item (LI) element
      UINT     reason;     // EVENT_REASON or EDIT_CHANGED_REASON - UI action causing change.
                           // In case of custom event notifications this may be any
                           // application specific value.
      SCITER_VALUE 
               data;       // auxiliary data accompanied with the event. E.g. FORM_SUBMIT event is using this field to pass collection of values.
  };

  struct TIMER_PARAMS
  {
      UINT_PTR timerId;    // timerId that was used to create timer by using HTMLayoutSetTimerEx
  };



  // identifiers of methods currently supported by intrinsic behaviors,
  // see function HTMLayoutCallMethod

  enum BEHAVIOR_METHOD_IDENTIFIERS
  {
    DO_CLICK = 0,
    GET_TEXT_VALUE = 1,
    SET_TEXT_VALUE,
      // p - TEXT_VALUE_PARAMS

    TEXT_EDIT_GET_SELECTION,
      // p - TEXT_EDIT_SELECTION_PARAMS

    TEXT_EDIT_SET_SELECTION,
      // p - TEXT_EDIT_SELECTION_PARAMS

    // Replace selection content or insert text at current caret position.
    // Replaced text will be selected.
    TEXT_EDIT_REPLACE_SELECTION,
      // p - TEXT_EDIT_REPLACE_SELECTION_PARAMS

    // Set value of type="vscrollbar"/"hscrollbar"
    SCROLL_BAR_GET_VALUE,
    SCROLL_BAR_SET_VALUE,

    FIRST_APPLICATION_METHOD_ID = 0x100
  };

  struct METHOD_PARAMS
  {
    UINT methodID; // see: #BEHAVIOR_METHOD_IDENTIFIERS
  };

  struct SCRIPTING_METHOD_PARAMS
  {
      LPCSTR        name;   //< method name
      SCITER_VALUE* argv;   //< vector of arguments
      UINT          argc;   //< argument count
      SCITER_VALUE  result; //< return value
  };

  #ifdef __cplusplus

  struct TEXT_VALUE_PARAMS: METHOD_PARAMS
  {
    LPCWSTR text;
    UINT    length;
    TEXT_VALUE_PARAMS(bool to_set = false) { methodID = to_set? SET_TEXT_VALUE: GET_TEXT_VALUE; }
  };

  struct TEXT_EDIT_SELECTION_PARAMS: METHOD_PARAMS
  {
    UINT selection_start;
    UINT selection_end;
    TEXT_EDIT_SELECTION_PARAMS(bool to_set = false) { methodID = to_set? TEXT_EDIT_SET_SELECTION: TEXT_EDIT_GET_SELECTION; }
  };

  struct TEXT_EDIT_REPLACE_SELECTION_PARAMS: METHOD_PARAMS
  {
    LPCWSTR text;
    UINT    text_length;
    TEXT_EDIT_REPLACE_SELECTION_PARAMS() { methodID = TEXT_EDIT_REPLACE_SELECTION; }
  };

  struct SCROLLBAR_VALUE_PARAMS: METHOD_PARAMS
  {
    INT     value;
    INT     min_value;
    INT     max_value;
    INT     page_value; // page increment
    INT     step_value; // step increment (arrow button click)
    SCROLLBAR_VALUE_PARAMS(bool to_set = false) { methodID = to_set? SCROLL_BAR_SET_VALUE: SCROLL_BAR_GET_VALUE; }
  };



  #endif


  // see HTMLayoutRequestElementData

  struct DATA_ARRIVED_PARAMS
  {
      HELEMENT  initiator;    // element intiator of HTMLayoutRequestElementData request,
      LPCBYTE   data;         // data buffer
      UINT      dataSize;     // size of data
      UINT      dataType;     // data type passed "as is" from HTMLayoutRequestElementData
  };




#pragma pack(pop)

#ifdef __cplusplus

  #pragma warning(disable:4786) //identifier was truncated...
  #pragma warning(disable:4996) //'strcpy' was declared deprecated
  #pragma warning(disable:4100) //unreferenced formal parameter

  namespace sciter
  {

    // event handler which can be attached to any DOM element.
    // event handler can be attached to the element as a "behavior" (see below)
    // or by sciter::dom::element::attach( event_handler* eh )

    struct event_handler
    {
      event_handler() // EVENT_GROUPS flags
      {
      }

      virtual void detached  (HELEMENT /*he*/ ) { }
      virtual void attached  (HELEMENT /*he*/ ) { }
      //virtual event_handler* attach (HELEMENT /*he*/ ) { return this; }

      // handlers with extended interface
      // by default they are calling old set of handlers (for compatibility with legacy code)

      /*virtual bool handle_mouse  (HELEMENT he, MOUSE_PARAMS& params )         { return false; }
      virtual bool handle_key    (HELEMENT he, KEY_PARAMS& params )           { return false; }
      virtual bool handle_focus  (HELEMENT he, FOCUS_PARAMS& params )         { return false; }
      virtual bool handle_timer  (HELEMENT he )                               { return false; }
      virtual void handle_size   (HELEMENT he )                               { }
      virtual bool handle_draw   (HELEMENT he, DRAW_PARAMS& params )          { return false; }
      virtual bool handle_method_call (HELEMENT he, METHOD_PARAMS& params )   { return false; }
      virtual bool handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params ) { return false; }
      virtual bool handle_data_arrived (HELEMENT he, DATA_ARRIVED_PARAMS& params ) { return false; }

      virtual bool handle_scripting_call(HELEMENT he, SCRIPTING_METHOD_PARAMS& params )
      {
        return false;
      }*/


      virtual bool handle_mouse  (HELEMENT he, MOUSE_PARAMS& params )
        {
          return on_mouse( he, params.target, params.cmd, params.pos, params.button_state, params.alt_state );
        }
      virtual bool handle_key    (HELEMENT he, KEY_PARAMS& params )
        {
          return on_key( he, params.target, params.cmd, params.key_code, params.alt_state );
        }
      virtual bool handle_focus  (HELEMENT he, FOCUS_PARAMS& params )
        {
          return on_focus( he, params.target, params.cmd );
        }
      virtual bool handle_timer  (HELEMENT he,TIMER_PARAMS& params )
        {
          if(params.timerId)
            return on_timer( he, params.timerId );
          return on_timer( he );
        }

      virtual void handle_size  (HELEMENT he )
        {
          on_size( he );
        }
      virtual bool handle_scroll  (HELEMENT he, SCROLL_PARAMS& params )
        {
          return on_scroll( he, params.target, (SCROLL_EVENTS)params.cmd, params.pos, params.vertical );
        }

      virtual bool handle_draw   (HELEMENT he, DRAW_PARAMS& params )
        {
          return on_draw(he, params.cmd, params.hdc, params.area );
        }

      virtual bool handle_method_call (HELEMENT he, METHOD_PARAMS& params )
        {
          return on_method_call(he, params.methodID, &params );
        }

      // notification events from builtin behaviors - synthesized events: BUTTON_CLICK, VALUE_CHANGED
      // see enum BEHAVIOR_EVENTS
      virtual bool handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params )
        {
          return on_event(he, params.heTarget, (BEHAVIOR_EVENTS)params.cmd, params.reason );
        }

      // notification event: data requested by HTMLayoutRequestData just delivered
      virtual bool handle_data_arrived (HELEMENT he, DATA_ARRIVED_PARAMS& params )
        {
          return on_data_arrived(he, params.initiator, params.data, params.dataSize, params.dataType );
        }

      virtual bool handle_scripting_call(HELEMENT he, SCRIPTING_METHOD_PARAMS& params )
        {
          return false;
        }

      //
      // alternative set of event handlers (aka old set).
      //
      virtual bool on_mouse  (HELEMENT he, HELEMENT target, UINT event_type, POINT pt, UINT mouseButtons, UINT keyboardStates ) { return FALSE; }
      virtual bool on_key    (HELEMENT he, HELEMENT target, UINT event_type, UINT code, UINT keyboardStates ) { return FALSE; }
      virtual bool on_focus  (HELEMENT he, HELEMENT target, UINT event_type ) { return FALSE; }
      virtual bool on_timer  (HELEMENT he ) { return FALSE; /*stop this timer*/ }
      virtual bool on_timer  (HELEMENT he, UINT_PTR extTimerId ) { return FALSE; /*stop this timer*/ }
      virtual bool on_draw   (HELEMENT he, UINT draw_type, HDC hdc, const RECT& rc ) { return FALSE; /*do default draw*/ }
      virtual void on_size   (HELEMENT he ) { }

      virtual bool on_method_call (HELEMENT he, UINT methodID, METHOD_PARAMS* params ) { return FALSE; /*not handled*/ }

      // calls from CSSS! script. Override this if you want your own methods to the CSSS! namespace.
      // Follwing declaration:
      // #my-active-on {
      //    when-click: r = self.my-method(1,"one");
      // }
      // will end up with on_script_call(he, "my-method" , 2, argv, retval );
      // where argv[0] will be 1 and argv[1] will be "one".
      virtual bool on_script_call(HELEMENT he, LPCSTR name, UINT argc, json::value* argv, json::value& retval) { return FALSE; }

      // notification events from builtin behaviors - synthesized events: BUTTON_CLICK, VALUE_CHANGED
      // see enum BEHAVIOR_EVENTS
      virtual bool on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason ) { return FALSE; }

      // notification event: data requested by HTMLayoutRequestData just delivered
      virtual bool on_data_arrived (HELEMENT he, HELEMENT initiator, LPCBYTE data, UINT dataSize, UINT dataType ) { return FALSE; }

      virtual bool on_scroll( HELEMENT he, HELEMENT target, SCROLL_EVENTS cmd, INT pos, BOOL isVertical ) { return FALSE; }

      // ElementEventProc implementeation
      static BOOL CALLBACK  element_proc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms )
      {
        event_handler* pThis = static_cast<event_handler*>(tag);
        if( pThis ) switch( evtg )
          {
            case HANDLE_INITIALIZATION:
              {
                INITIALIZATION_PARAMS *p = (INITIALIZATION_PARAMS *)prms;
                if(p->cmd == BEHAVIOR_DETACH)
                  pThis->detached(he);
                else if(p->cmd == BEHAVIOR_ATTACH)
                  pThis->attached(he);
                return TRUE;
              }
            case HANDLE_MOUSE: {  MOUSE_PARAMS *p = (MOUSE_PARAMS *)prms; return pThis->handle_mouse( he, *p );  }
            case HANDLE_KEY:   {  KEY_PARAMS *p = (KEY_PARAMS *)prms; return pThis->handle_key( he, *p ); }
            case HANDLE_FOCUS: {  FOCUS_PARAMS *p = (FOCUS_PARAMS *)prms; return pThis->handle_focus( he, *p ); }
            case HANDLE_DRAW:  {  DRAW_PARAMS *p = (DRAW_PARAMS *)prms; return pThis->handle_draw(he, *p ); }
            case HANDLE_TIMER: {  TIMER_PARAMS *p = (TIMER_PARAMS *)prms; return pThis->handle_timer(he, *p); }
            case HANDLE_BEHAVIOR_EVENT:   { BEHAVIOR_EVENT_PARAMS *p = (BEHAVIOR_EVENT_PARAMS *)prms; return pThis->handle_event(he, *p ); }
            case HANDLE_METHOD_CALL:      { METHOD_PARAMS *p = (METHOD_PARAMS *)prms; return pThis->handle_method_call(he, *p ); }
            case HANDLE_DATA_ARRIVED:     { DATA_ARRIVED_PARAMS *p = (DATA_ARRIVED_PARAMS *)prms; return pThis->handle_data_arrived(he, *p ); }
            case HANDLE_SIZE:  {  pThis->handle_size(he); return FALSE; }
            case HANDLE_SCRIPTING_METHOD_CALL: { SCRIPTING_METHOD_PARAMS* p = (SCRIPTING_METHOD_PARAMS *)prms; return pThis->handle_scripting_call(he, *p ); }

            case HANDLE_SCROLL:

              return FALSE; // for a while they are empty

            default:
              assert(false);
          }
        return FALSE;
      }
    };


    //
    // "behavior" is a named event_handler
    // behaviors organized into one global list to be processed
    // automaticly while handling HLN_ATTACH_BEHAVIOR notification
    //

    struct behavior_factory
    {
      behavior_factory(const char* external_name)
        :next(0),name(external_name)
      {
        // add this implementation to the list (singleton)
        next = root();
        root(this);
      }

      // needs to be overriden
      virtual event_handler* create(HELEMENT he) = 0;

      // behavior list support
      behavior_factory* next;
      const char*       name; // name must be a pointer to a static string

      // returns behavior implementation by name.
      static event_handler* create(const char* name, HELEMENT he)
      {
        for(behavior_factory* t = root(); t; t = t->next)
          if(strcmp(t->name,name)==0)
          {
            return t->create(he);
          }
        return 0; // not found
      }
      // implementation of static list of behaviors
      static behavior_factory* root(behavior_factory* to_set = 0)
      {
        static behavior_factory* _root = 0;
        if(to_set) _root = to_set;
        return _root;
      }

    };

    inline void attach_dom_event_handler(HWND hwnd, event_handler* ph)
    {
      int r = SciterWindowAttachEventHandler( hwnd, &event_handler::element_proc, ph, HANDLE_ALL );
      assert(r == SCDOM_OK); r;
    }
    inline void detach_dom_event_handler(HWND hwnd, event_handler* ph)
    {
      int r = SciterWindowDetachEventHandler( hwnd, &event_handler::element_proc, ph );
      assert(r == SCDOM_OK); r;
    }


  } //namespace sciter

#endif //__cplusplus




#endif

