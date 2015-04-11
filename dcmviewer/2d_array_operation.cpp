#include "StdAfx.h"
#include "2d_array_operation.h"

void flip_array(UINT8* dibits,int m_rows, int m_cols,int bytes_per_pixel)
{
	//exchange row i & (m_rows-i)
	if(NULL==dibits || 0>= m_rows || 0>= m_cols || 0>=bytes_per_pixel)
		return;
	int bytes_of_a_row = m_cols * bytes_per_pixel;
	int bottom_row = m_rows - 1;
	std::auto_ptr<UINT8> tmpbuf(new UINT8[bytes_of_a_row]);
	if(NULL==tmpbuf.get())
		return;
	for(int i=0;i<m_rows/2;i++)
	{
		memcpy(tmpbuf.get(),dibits+i*bytes_of_a_row,bytes_of_a_row); //backup  row i
		memcpy(dibits + i*bytes_of_a_row, dibits
			+(bottom_row-i)*bytes_of_a_row,bytes_of_a_row);//put m_rows-i to row i
		memcpy(dibits +(bottom_row-i)*bytes_of_a_row,tmpbuf.get(), bytes_of_a_row);//put old row i to m_rows-i
	}
}