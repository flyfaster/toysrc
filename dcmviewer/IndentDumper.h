#pragma	   once
struct IndentDumper
{
	IndentDumper(std::ostream& Out,int level):Out_(Out),m_indent(level)
	{
	}
	void operator()(const dicom::DataSet::value_type& v);
	static void IndentDump(const dicom::DataSet &data,std::ostream &out,int indent_level=0)
	{
		//std::for_each(data.begin(),data.end(),IndentDumper(Out,level));
		dicom::DataSet::const_iterator it = data.begin();
		while(it!=data.end() )
		{
			dicom::Tag t = it->first;
			IndentDumper::write_tag(out,t,indent_level);
			int max_vm_print = 16;
			dicom::VR vr_last;
			while( it!=data.end() && it->first == t)
			{
				const dicom::VR vr = it->second.vr();
				vr_last = vr;
				if(dicom::VR_OW!=vr && dicom::VR_OB!=vr)
				{
					if(max_vm_print<16)
						out<<"\\";
					if(max_vm_print>0)
						out<< it->second ;
				}
				else
				{
					IndentDumper(out,indent_level).operator ()(*it);
				}
				it++;
				max_vm_print--;
			}
			if(dicom::VR_OW!=vr_last && dicom::VR_OB!=vr_last)
				IndentDumper::write_eol(out);
		}
	} 
	static void write_tag(std::ostream &Out_,const dicom::Tag &tag,int indent_count)
	{
		for(int i=0;i<indent_count;i++)
			//Out_<<indent_char;
			write_indent(Out_);
		Out_ <<"("<< std::setw(4)<<std::setfill('0')<<std::hex<<GroupTag(tag)<<",";
		Out_ <<std::setw(4)<<std::setfill('0')<<ElementTag(tag)<<")  ";
		Out_ <<GetVRShortName(dicom::GetVR(tag))<<"  ";
		Out_ << GetName(tag) << "    "<<std::dec ;

	}
	static void write_eol(std::ostream& Out)
	{
		Out<< char(0xd)<<char(0xa);
	}
private:
	int m_indent;
	std::ostream& Out_;
	//static const std::string indent_char = '    ';
	static void write_indent(std::ostream& Out)
	{
		Out<< "    ";
	}
	void write_tag(const dicom::Tag &tag,int indent_count)
	{
		write_tag(Out_,tag,indent_count);
	}
};

struct IndentDumpDataSet
{
	std::ostream& out;
	int m_indent;
	IndentDumpDataSet(std::ostream& os,int indent):out(os),m_indent(indent){}
	void operator()(const dicom::DataSet& data)
	{
		//for_each(data.begin(),data.end(),IndentDumper(out,m_indent));
		IndentDumper::IndentDump(data,out,m_indent);
		//dicom::DataSet::const_iterator it = data.begin();
		//while(it!=data.end() )
		//{
		//	dicom::Tag t = it->first;
		//	IndentDumper::write_tag(out,t,m_indent);
		//	int max_vm_print = 16;
		//	while(it->first == t)
		//	{
		//		const dicom::VR vr = it->second.vr_;
		//		if(dicom::VR_OW!=vr && dicom::VR_OB!=vr)
		//		{
		//			if(max_vm_print<16)
		//				out<<"\\";
		//			if(max_vm_print>0)
		//				out<< it->second ;
		//		}
		//		else
		//		{
		//			IndentDumper(out,m_indent).operator ()(*it);
		//		}
		//		it++;
		//		max_vm_print--;
		//	}
		//	IndentDumper::write_eol(out);
		//}
	}
};