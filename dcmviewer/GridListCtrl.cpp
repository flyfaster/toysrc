// GridListCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "GridListCtrl.h"
#include <memory>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridListCtrl

CGridListCtrl::CGridListCtrl()
	{
		m_pListEdit = NULL;
	m_CurSubItem = -1;
	m_bEditPerCol = false;
	}

CGridListCtrl::~CGridListCtrl()
	{
		if(m_pListEdit)
			delete m_pListEdit;
	}


BEGIN_MESSAGE_MAP(CGridListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CGridListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridListCtrl message handlers

BOOL CGridListCtrl::PrepareControl(WORD wStyle)
	{
	m_wStyle = wStyle;
	ASSERT( m_hWnd );
    DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE); 
	dwStyle &= ~(LVS_TYPEMASK);
	dwStyle &= ~(LVS_EDITLABELS);
 
	// Make sure we have report view and send edit label messages.
    SetWindowLong( m_hWnd, GWL_STYLE, dwStyle | LVS_REPORT );

	// Enable the full row selection and the drag drop of headers.
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP ;
	// Use macro since this is new and not in MFC.
	ListView_SetExtendedListViewStyleEx(m_hWnd, styles, styles );
	return TRUE;
	}

void CGridListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
	{
	LVHITTESTINFO ht;
	ht.pt = point;
	// Test for which subitem was clicked.
	// Use macro since this is new and not in MFC.
	int rval = ListView_SubItemHitTest( m_hWnd, &ht );

	// Store the old column number and set the new column value.
	int oldsubitem = m_CurSubItem;
	m_CurSubItem = IndexToOrder( ht.iSubItem );

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	// Make the column fully visible.
	// We have to take into account that the columns may be reordered
	MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );

	// Store old state of the item.
	int state = GetItemState( ht.iItem, LVIS_FOCUSED );

	// Call default left button click is here just before we might bail.
	// Also updates the state of the item.
	CListCtrl::OnLButtonDown(nFlags, point);

	// Bail if the state from before was not focused or the 
	// user has not already clicked on this cell.
	if( !state 
		|| m_CurSubItem == -1 
		|| oldsubitem != m_CurSubItem ) return;

	int doedit = 0;
	// If we are in column 0 make sure that the user clicked on 
	// the item label.
	if( 0 == ht.iSubItem )
		{
		if( ht.flags & LVHT_ONITEMLABEL ) doedit = 1;
		}
	else
		{
		doedit = 1;
		}
	if( !doedit ) return;

    // Send Notification to parent of ListView ctrl
	if(IsColEditable(ht.iSubItem)==false)
		return;
	CString str;
	str = GetItemText( ht.iItem, ht.iSubItem );
    LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_BEGINLABELEDIT;
	
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = ht.iItem;
	dispinfo.item.iSubItem = ht.iSubItem;
	dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	
	GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), 
		(LPARAM)&dispinfo );
	}

BOOL CGridListCtrl::PositionControl( CWnd * pWnd, int iItem, int iSubItem )
	{
	ASSERT( pWnd && pWnd->m_hWnd );
	ASSERT( iItem >= 0 );
	// Make sure that the item is visible
	if( !EnsureVisible( iItem, TRUE ) ) return NULL;
	
	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	ASSERT( iSubItem >= 0 && iSubItem < nColumnCount );
	if( iSubItem >= nColumnCount || 
		// We have to take into account that the header may be reordered
		GetColumnWidth(Header_OrderToIndex( pHeader->m_hWnd,iSubItem)) < 5 )
		{
		return 0;
		}
	
	// Get the header order array to sum the column widths up to the selected cell.
	int *orderarray = new int[ nColumnCount ];
	Header_GetOrderArray( pHeader->m_hWnd, nColumnCount, orderarray );
	int offset = 0;
	int i;
	for( i = 0; orderarray[i] != iSubItem; i++ )
		offset += GetColumnWidth( orderarray[i] );
	int colwidth = GetColumnWidth( iSubItem );
	delete[] orderarray;
	
	CRect rect;
	GetItemRect( iItem, &rect, LVIR_BOUNDS );
	
	// Scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right )
        {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
        }
	
	rect.left += offset+4;
	rect.right = rect.left + colwidth - 3 ;
	// The right end of the control should not go past the edge 
	// of the grid control.
	if( rect.right > rcClient.right) 
		rect.right = rcClient.right;
	pWnd->MoveWindow( &rect );

	return 1;	
	}

void CGridListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	// This function is called by the control in different 
	// stages during the control drawing process.

	NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// By default set the return value to do the default behavior.
	*pResult = 0;

	switch( pCD->nmcd.dwDrawStage )
		{
		case  CDDS_PREPAINT:  // First stage (for the whole control)
			// Tell the control we want to receive drawing messages  
			// for drawing items.
			*pResult = CDRF_NOTIFYITEMDRAW;
			// The next stage is handled in the default:
			break;
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM: // Stage three (called for each subitem of the focused item)
			{
			// We don't want to draw anything here, but we need to respond 
			// of DODEFAULT will be the next stage.
			// Tell the control we want to handle drawing after the subitem 
			// is drawn.
			*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
			}
			break;
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
			{
			// We do the drawing here (well maybe).
			// This is actually after the control has done its drawing
			// on the subitem.  Since drawing a cell is near instantaneous
			// the user won't notice.
			int subitem = pCD->iSubItem;
			// Only do our own drawing if this subitem has focus at the item level.
			if( (pCD->nmcd.uItemState & CDIS_FOCUS) )
				{
				// If this subitem is the subitem with the current focus,
				// draw it.  Otherwise let the control draw it.  
				CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
				// We have to take into account the possibility that the 
				// columns may be reordered.
				if( subitem == Header_OrderToIndex( pHeader->m_hWnd,  m_CurSubItem ) )
					{
					// POSTERASE
					CDC* pDC = CDC::FromHandle(pCD->nmcd.hdc);
					// Calculate the offset of the text from the right and left of the cell.
					int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;
					// The rect for the cell gives correct left and right values.
					CRect rect = pCD->nmcd.rc;
					CRect bounds;
					GetItemRect( pCD->nmcd.dwItemSpec, &bounds, LVIR_BOUNDS );
					// Get the top and bottom from the item itself.
					rect.top = bounds.top;
					rect.bottom = bounds.bottom;
					// Adjust rectangle for horizontal scroll and first column label
					{
					if( subitem == 0 )
						{
						CRect lrect;
						GetItemRect( pCD->nmcd.dwItemSpec, &lrect, LVIR_LABEL );
						rect.left = lrect.left;
						rect.right = lrect.right;
						}
					else
						{
						rect.right += bounds.left;
						rect.left  += bounds.left;
						}
					}
					// Clear the background with button face color
					pDC->FillRect(rect, &CBrush(::GetSysColor(COLOR_3DFACE)));
					// PREPAINT
					CString str;
					str = GetItemText( pCD->nmcd.dwItemSpec, pCD->iSubItem );
					// Deflate the rect by the horizontal offset.
					rect.DeflateRect( offset, 0 );
					// You could also make this column alignment sensitive here.
					pDC->DrawText( str, rect, 
						DT_SINGLELINE|DT_NOPREFIX|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS);
					// POSTPAINT
					// Draw rounded edge
					rect.InflateRect( offset, 0 );
					pDC->Draw3dRect( &rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DFACE) );
					rect.DeflateRect( 1, 1 );
					pDC->Draw3dRect( &rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHILIGHT) );
					// Tell the control that we handled the drawing for this subitem.
					*pResult = CDRF_SKIPDEFAULT;
					}
				}
			}
			break;
		default: // Stage two handled here. (called for each item)
			if( !(pCD->nmcd.uItemState & CDIS_FOCUS) )
				{
				// If this item does not have focus, let the 
				// control draw the whole item.
				*pResult = CDRF_DODEFAULT;
				}
			else
				{
				// If this item has focus, tell the control we want
				// to handle subitem drawing.
				*pResult = CDRF_NOTIFYSUBITEMDRAW;
				}
			break;
		}
	}

void CGridListCtrl::MakeColumnVisible(int nCol)
	{
	if( nCol < 0 )
		return;
	// Get the order array to total the column offset.
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int colcount = pHeader->GetItemCount();
	ASSERT( nCol < colcount );
	int *orderarray = new int[ colcount ];
	Header_GetOrderArray( pHeader->m_hWnd, colcount, orderarray );
	// Get the column offset
	int offset = 0;
	for( int i = 0; orderarray[i] != nCol; i++ )
		offset += GetColumnWidth( orderarray[i] );
	int colwidth = GetColumnWidth( nCol );
	delete[] orderarray;
	
	CRect rect;
	GetItemRect( 0, &rect, LVIR_BOUNDS );
	
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right )
        {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
        }
	}

BOOL CGridListCtrl::PreTranslateMessage(MSG* pMsg) 
    {
    if(pMsg->message == WM_KEYDOWN)
        {
        // Handle the keystrokes for the left and right keys
        // to move the cell selection left and right.
        // Handle F2 to commence edit mode from the keyboard.
        // Only handle these if the grid control has the focus.
        // (Messages also come through here for the edit control
        // and we don't want them.
        if( this == GetFocus() )
            {
            switch( pMsg->wParam )
                {
                case VK_LEFT:
                    {
                    // Decrement the order number.
                    m_CurSubItem--;
                    if( m_CurSubItem < -1 ) 
                        {
                        // This indicates that the whole row is selected and F2 means nothing.
                        m_CurSubItem = -1;
                        }
                    else
                        {
                        CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
                        // Make the column visible.
                        // We have to take into account that the header
                        // may be reordered.
                        MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
                        // Invalidate the item.
                        int iItem = GetNextItem( -1, LVNI_FOCUSED );
                        if( iItem != -1 )
                            {
                            CRect rcBounds;
                            GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
                            InvalidateRect( &rcBounds );
                            }
                        }
                    }
                    return TRUE;
                case VK_RIGHT:
                    {
                    // Increment the order number.
                    m_CurSubItem++;
                    CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
                    int nColumnCount = pHeader->GetItemCount();
                    // Don't go beyond the last column.
                    if( m_CurSubItem > nColumnCount -1 ) 
                        {
                        m_CurSubItem = nColumnCount-1;
                        }
                    else
                        {
                        // We have to take into account that the header
                        // may be reordered.
                        MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
                        int iItem = GetNextItem( -1, LVNI_FOCUSED );
                        // Invalidate the item.
                        if( iItem != -1 )
                            {
                            CRect rcBounds;
                            GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
                            InvalidateRect( &rcBounds );
                            }
                        }
                    }
                    return TRUE;
                case VK_F2: // Enter nondestructive edit mode.
                    {
                    int iItem = GetNextItem( -1, LVNI_FOCUSED );
                    if( m_CurSubItem != -1 && iItem != -1 )
                        {
                        // Send Notification to parent of ListView ctrl
                        CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
                        CString str;
                        // We have to take into account that the header
                        // may be reordered.
                        str = GetItemText( iItem, Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem ) );
                        LV_DISPINFO dispinfo;
                        dispinfo.hdr.hwndFrom = m_hWnd;
                        dispinfo.hdr.idFrom = GetDlgCtrlID();
                        dispinfo.hdr.code = LVN_BEGINLABELEDIT;
                        
                        dispinfo.item.mask = LVIF_TEXT;
                        dispinfo.item.iItem = iItem;
                        // We have to take into account that the header
                        // may be reordered.
                        dispinfo.item.iSubItem = Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem );
                        dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
                        dispinfo.item.cchTextMax = str.GetLength();
                        // Send message to the parent that we are ready to edit.
                        GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), 
                            (LPARAM)&dispinfo );
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    
    return CListCtrl::PreTranslateMessage(pMsg);
    }


int CGridListCtrl::IndexToOrder( int iIndex )
	{
	// Since the control only provide the OrderToIndex macro,
	// we have to provide the IndexToOrder.  This translates
	// a column index value to a column order value.
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int colcount = pHeader->GetItemCount();
	std::auto_ptr<int> orderarray(new int[ colcount ]);
	//int *orderarray = new int[ colcount ];
	Header_GetOrderArray( pHeader->m_hWnd, colcount, orderarray.get() );
	int i;
	for( i=0; i<colcount; i++ )
		{
		if( orderarray.get()[i] == iIndex )
			return i;
		}
	return -1;
	}

void CGridListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CGridListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	}
	void CGridListCtrl::OnLvnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

		CString str = pDispInfo->item.pszText;
		int item = pDispInfo->item.iItem;
		int subitem = pDispInfo->item.iSubItem;
		if(IsColEditable(subitem)==false)
			return;
		// Construct and create the custom multiline edit control.
		// We could just as well have used a combobox, checkbox, 
		// rich text control, etc.
		if(m_pListEdit) 
		{
			delete m_pListEdit;
			m_pListEdit=NULL;
		}
		m_pListEdit = new CInPlaceEdit( item, subitem, str );
		// Start with a small rectangle.  We'll change it later.
		CRect  rect( 0,0,1,1 );
		DWORD dwStyle = ES_LEFT;
		dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL;
		m_pListEdit->Create( dwStyle, rect,this/* &m_GridListCtrl*/, 103 );
		// Have the Grid position and size the custom edit control
		/*m_GridListCtrl->*/PositionControl( m_pListEdit, item, subitem );
		// Have the edit box size itself to its content.
		m_pListEdit->CalculateSize();
		// Return TRUE so that the list control will hnadle NOT edit label itself. 
		*pResult = TRUE;
	}

	void CGridListCtrl::OnLvnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
		// TODO: Add your control notification handler code here
		int item = pDispInfo->item.iItem;
		int subitem = pDispInfo->item.iSubItem;
		// This is coming from the grid list control notification.
		if( m_pListEdit )
		{
			CString str;
			if( pDispInfo->item.pszText )
				this->SetItemText( item, subitem, pDispInfo->item.pszText );
			delete m_pListEdit;
			m_pListEdit = 0;
		}
		*pResult = TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////
	// CInPlaceEdit

	CInPlaceEdit::CInPlaceEdit(int iItem, int iSubItem, CString sInitText)
		:m_sInitText( sInitText )
	{
		m_iItem = iItem;
		m_iSubItem = iSubItem;
		m_bESC = FALSE;
	}

	CInPlaceEdit::~CInPlaceEdit()
	{
	}


	BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
		//{{AFX_MSG_MAP(CInPlaceEdit)
		ON_WM_KILLFOCUS()
		ON_WM_CHAR()
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CInPlaceEdit message handlers

	BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			SHORT sKey = GetKeyState( VK_CONTROL);
			if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| sKey
				)
			{
				::TranslateMessage(pMsg);
				/* Strange but true:
				If the edit control has ES_MULTILINE and ESC
				is pressed the parent is destroyed if the 
				message is dispatched.  In this 
				case the parent is the list control. */
				if( !(GetStyle() & ES_MULTILINE) || pMsg->wParam != VK_ESCAPE )
				{
					::DispatchMessage(pMsg);
				}
				return TRUE;                    // DO NOT process further
			}
		}

		return CEdit::PreTranslateMessage(pMsg);
	}


	void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
	{
		CEdit::OnKillFocus(pNewWnd);

		CString str;
		GetWindowText(str);

		// Send Notification to parent of ListView ctrl
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;

		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = m_iItem;
		dispinfo.item.iSubItem = m_iSubItem;
		dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
		dispinfo.item.cchTextMax = m_bESC ? 0 : str.GetLength();

		GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), 
			(LPARAM)&dispinfo );

	}


	void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if( nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
			if( nChar == VK_ESCAPE )
				m_bESC = TRUE;
			GetParent()->SetFocus();
			return;
		}

		CEdit::OnChar(nChar, nRepCnt, nFlags);

		// Resize edit control if needed
		CalculateSize();
	}

	int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CEdit::OnCreate(lpCreateStruct) == -1)
			return -1;

		// Set the proper font
		CFont* font = GetParent()->GetFont();
		SetFont(font);

		SetWindowText( m_sInitText );
		SetFocus();
		CalculateSize();
		SetSel( 0, -1 );
		return 0;
	}

	void CInPlaceEdit::CalculateSize()
	{
		// Get text extent
		CString str;

		GetWindowText( str );
		CWindowDC dc(this);
		CFont *pFont = GetParent()->GetFont();
		CFont *pFontDC = dc.SelectObject( pFont );
		CSize size;

		// Get client rect
		CRect rect, parentrect;
		GetClientRect( &rect );
		GetParent()->GetClientRect( &parentrect );

		// Transform rect to parent coordinates
		ClientToScreen( &rect );
		GetParent()->ScreenToClient( &rect );

		if( !(GetStyle() & ES_MULTILINE ) )
		{
			size = dc.GetTextExtent( str );
			dc.SelectObject( pFontDC );
			size.cx += 5;                           // add some extra buffer
		}
		else
		{
			CRect thinrect( rect );  // To measure the skinniest text box
			CRect widerect( rect );  // To measure the wides text box
			widerect.right = parentrect.right;
			// Use the shortest of the two box sizes.
			int thinheight = dc.DrawText( str, &thinrect, DT_CALCRECT|DT_NOPREFIX|DT_LEFT|DT_EXPANDTABS|DT_WORDBREAK );
			int wideheight = dc.DrawText( str, &widerect, DT_CALCRECT|DT_NOPREFIX|DT_LEFT|DT_EXPANDTABS|DT_WORDBREAK );
			if( thinheight >= wideheight )
			{
				size.cy = wideheight + 5;
				size.cx = widerect.right - widerect.left + 5;
			}
			else
			{
				size.cy = thinheight + 5;
				size.cx = thinrect.right - thinrect.left + 5;
			}
		}

		// Check whether control needs to be resized
		// and whether there is space to grow
		int changed = 0;
		if( size.cx > rect.Width() )
		{
			if( size.cx + rect.left < parentrect.right-2 )
				rect.right = rect.left + size.cx;
			else
				rect.right = parentrect.right-2;
			changed = 1;
		}
		if( size.cy > rect.Height() )
		{
			if( size.cy + rect.top < parentrect.bottom-2 )
				rect.bottom = rect.top + size.cy;
			else
			{
				rect.bottom = parentrect.bottom-2;
				ShowScrollBar( SB_VERT );
			}
			changed = 1;
		}
		// If the size became larger rposition the window.
		if( changed )
			MoveWindow( &rect );
	}




	void CGridListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		return;
		int nSelection=0;
		CMenu menu;
		POINT curPos;
		const int IDM_REVERSE_SEL =1234;
		const int IDM_SELECT_ALL =IDM_REVERSE_SEL+1;
		const int IDM_CLEAR_ALL =IDM_SELECT_ALL+1;
		const int IDM_REMOVE_ALL = IDM_CLEAR_ALL +1;
		const int IDM_REMOVE_UNSELECTED = IDM_REMOVE_ALL+1;
		const int IDM_REMOVE_SELECTED = IDM_REMOVE_UNSELECTED+1;
		menu.CreatePopupMenu(); 
		menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_CLEAR_ALL,_T("Clear all selection"));
		menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_REVERSE_SEL,_T("Reverse selection"));
		menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_SELECT_ALL,_T("Select all rows"));
		menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_REMOVE_ALL,_T("Remove all rows"));
		menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_REMOVE_UNSELECTED,_T("Remove unselected rows"));
		menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_REMOVE_SELECTED,_T("Remove selected rows"));

		GetCursorPos(&curPos);
		SetForegroundWindow();
		/*
		MSG	msg;
		while(PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
		*/
		nSelection=menu.TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN | 
			TPM_BOTTOMALIGN|TPM_NONOTIFY|TPM_RETURNCMD, curPos.x, curPos.y, this);
		::PostMessage(m_hWnd, 0, 0, 0);
		menu.DestroyMenu(); 
		int ncount = GetItemCount();
		switch(nSelection) {
		case IDM_CLEAR_ALL:
			for(int i=0;i<ncount;i++)
				SetCheck(i,FALSE);
			break;
		case IDM_SELECT_ALL:
			for(int i=0;i<ncount;i++)
				SetCheck(i,TRUE);
			break;
		case IDM_REVERSE_SEL:
			{
				BOOL bsel = TRUE;
				for(int i=0;i<ncount;i++)
				{
					bsel = GetCheck(i);
					SetCheck(i,!bsel);
				}
			}
			break;
		case IDM_REMOVE_ALL:
			this->DeleteAllItems();
			break;
		case IDM_REMOVE_SELECTED:
			for(int i=ncount-1;i>=0;i--)
			{
				BOOL bsel=GetCheck(i);
				if(bsel)
					this->DeleteItem(i);
			}
			break;
		case IDM_REMOVE_UNSELECTED:
			for(int i=ncount-1;i>=0;i--)
			{
				BOOL bsel=GetCheck(i);
				if(!bsel)
					this->DeleteItem(i);
			}
			break;
		default:
			break;
		}

		CListCtrl::OnRButtonDown(nFlags, point);
	}

	void CGridListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default

		CListCtrl::OnRButtonUp(nFlags, point);
	}

	void CGridListCtrl::ResetEditableCols(void)
	{
		m_bEditPerCol = true;
		m_qEditableCols.clear();
	}

	void CGridListCtrl::AddEditableCol(int columnindex)
	{
		m_bEditPerCol = true;
		if(std::find(m_qEditableCols.begin(),m_qEditableCols.end(),columnindex)==m_qEditableCols.end())
			m_qEditableCols.push_back(columnindex);
	}

	bool CGridListCtrl::IsColEditable(int columnindex)
	{
		bool bItemEditable = false;
		if(!m_bEditPerCol)
			bItemEditable =  true;
		if(std::find(m_qEditableCols.begin(),m_qEditableCols.end(),columnindex)!=m_qEditableCols.end())
			bItemEditable =  true;
		//AtlTrace("%s(%d)return %d",__TFUNCTION__,columnindex,bItemEditable);
		return bItemEditable;
	}
	//tchar_string trim( const tchar_string& strSource);
	tchar_string trim( const tchar_string& strSource)
	{
		int nLen = ( int)strSource.length();
		int idxFirst = 0;
		int idxLast = nLen - 1;
		while ( (idxFirst < nLen) && _istspace( strSource[idxFirst]))
		{ idxFirst++; }
		while ( ( idxLast >= idxFirst) && _istspace( strSource[idxLast]))
		{ idxLast--; }
		if ( idxLast >= idxFirst)
		{
			tchar_string strTrimmed = strSource.substr( idxFirst,
				idxLast + 1 -idxFirst);
			return strTrimmed;
		}
		else
		{
			// this string contains only spaces
			return tchar_string();
		}
	}
	int CGridListCtrl::GetColIndexByName(tchar_string col_name)
	{
		//return -1 if error
		if(!GetHeaderCtrl())
			return -1;
		int ncount = GetHeaderCtrl()->GetItemCount();
		HDITEM hdi;
		TCHAR  lpBuffer[256];
		bool   fFound = false;

		hdi.mask = HDI_TEXT;
		hdi.pszText = lpBuffer;
		hdi.cchTextMax = 256;

		for(int i=0;i<ncount; i++)
		{
			//HDITEM hd;
			GetHeaderCtrl()->GetItem(i,&hdi);
			if( _tcscmp(hdi.pszText, trim(col_name).c_str())==0)
			{
				return i;
			}
		}
		return -1;
	}
