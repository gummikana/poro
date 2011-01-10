#ifndef INC_POINTER_SORTER
#define INC_POINTER_SORTER

namespace ceng {

struct pointer_sorter
{
	template< class T >
	bool operator()( const T* a, const T* b ) const 
	{
		if (a == 0) 
		{
			return b != 0; 
		} 
		else if (b == 0) 
		{
			return false;
		}
		else
		{
			return (*a) < (*b);
		}
	}

	template< class T >
	bool operator()( const T* a, const T* b ) 	
	{
		if (a == 0) 
		{
			return b != 0; 
		} 
		else if (b == 0) 
		{
			return false;
		}
		else
		{
			return (*a) < (*b);
		}
	}

};

} //end of namespace ceng
#endif
