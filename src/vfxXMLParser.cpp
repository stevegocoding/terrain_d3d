#include "vfxxmlparser.h"
#include <iostream>
#include <sstream>
#include <string>
#include <strstream>
#include <tchar.h>


static const char chXMLTagOpen		= '<';
static const char chXMLTagClose	= '>';
static const char chXMLTagPre	= '/';
//static const char chXMLEscape = '\\';	// for value field escape
static const char chXMLEscape = '$';	// for value field escape

// default entity table
static const XENTITY x_EntityTable[] = {
	{ '&', "&amp;", 5 } ,
	{ '\"', "&quot;", 6 } ,
	{ '\'', "&apos;", 6 } ,
	{ '<', "&lt;", 4 } ,
	{ '>', "&gt;", 4 } 
};

XENTITYS vfxXMLParser::entityDefault = XENTITYS((LPXENTITY)x_EntityTable, sizeof(x_EntityTable)/sizeof(x_EntityTable[0]));
struct _tagDispOption vfxXMLParser::optDefault;
struct _tagParseInfo vfxXMLParser::piDefault;


//========================================================
// Desc   : same with _tcspbrk 
//========================================================
char* _tcschrs( const char* psz, const char* pszchs )
{
	while( psz && *psz )
	{
		if( strchr( pszchs, *psz ) )
			return (char*)psz;
		psz++;
	}
	return NULL;
}

//========================================================
// Desc   : skip space
//========================================================
char* _tcsskip( const char* psz )
{
	while( psz && *psz == ' ' ) psz++;

	return (char*)psz;
}

//========================================================
// Desc   : skip return
//========================================================
char* _tcrskip( const char* psz )
{
	while( psz && (*psz == '\r' || *psz == '\n')) psz++;

	return (char*)psz;
}

//========================================================
// Desc   : skip tab
//========================================================
char* _tctskip( const char* psz )
{
	while( psz && *psz == '\t') psz++;

	return (char*)psz;
}



//========================================================
// Desc   : similar with _tcschr with escape process
// Param  : escape - will be escape character
//========================================================
char* _tcsechr( const char* psz, int ch, int escape )
{
	char* pch = (char*)psz;
	while( pch && *pch )
	{
		if( *pch == escape )
			pch++;
		else
			if( *pch == ch )
				return (char*)pch;
		pch++;
	}
	return pch;
}

//========================================================
// Desc   : similar with _tcslen with escape process
// Param  : escape - will be escape character
//========================================================
int _tcselen( int escape, char* srt, char* end = NULL ) 
{
	int len = 0;
	char* pch = srt;
	if( end==NULL ) end = (char*)sizeof(long);
	while( pch && *pch && pch<end )
	{
		if( *pch != escape )
			len++;
		pch++;
	}
	return len;
}

//========================================================
// Desc   : similar with _tcscpy with escape process
// Param  : escape - will be escape character
//========================================================
void _tcsecpy( char* psz, int escape, char* srt, char* end = NULL )
{
	char* pch = srt;
	if( end==NULL ) end = (char*)sizeof(long);
	while( pch && *pch && pch<end )
	{
		if( *pch != escape )
			*psz++ = *pch;
		pch++;
	}

	*psz = '\0';
}

//========================================================
// Desc   : similar with _tcspbrk with escape process
// Param  : escape - will be escape character
//========================================================
char* _tcsepbrk( const char* psz, const char* chset, int escape )
{
	char* pch = (char*)psz;
	while( pch && *pch )
	{
		if( *pch == escape )
			pch++;
		else
			if( _tcschr( chset, *pch ) )
				return (char*)pch;		
		pch++;
	}
	return pch;
}

//========================================================
// Desc   : put std::string of (psz~end) on ps std::string
// Param  : trim - will be trim?
//========================================================
void _Setstring( char* psz, char* end, std::string* ps, bool trim = FALSE, int escape = 0 )
{
	//trim
	if( trim )
	{
		while( psz && psz < end && (*psz == ' ' || *psz == 9 ) ) psz++;
		while( (end-1) && psz < (end-1) && (*(end-1) == ' ' || *(end-1) == 9 ) ) end--;
	}
	int len = (int)(end - psz);
	if( len <= 0 ) return;

	if( escape )
	{
		len = _tcselen( escape, psz, end );
		//char* pss = ps->GetBufferSetLength( len );
		//_tcsecpy( pss, escape, psz, end );
		ps->assign ( psz , len );
	}
	else
	{
		ps->assign ( psz , len );
		//char* pss = ps->GetBufferSetLength(len + 1 );
		//memcpy( pss, psz, len );
		//pss[len] = '\0';
	}
}


_tagXMLNode::~_tagXMLNode()
{
	for( int i = 0 ; i < (int)childs.size(); i ++)
	{
		LPXNode p = childs[i];
		if( p )
		{
			delete p; childs[i] = NULL;
		}
	}
	childs.clear();

	for( int i = 0 ; i < (int)attrs.size(); i ++)
	{
		LPXAttr p = attrs[i];
		if( p )
		{
			delete p; attrs[i] = NULL;
		}
	}
	attrs.clear();
}

// attr1="value1" attr2='value2' attr3=value3 />
// ^- return pointer
//========================================================
// Desc   : loading attribute plain xml text
// Param  : pszAttrs - xml of attributes
//          pi = parser information
// Return : advanced std::string pointer.
//========================================================
char* _tagXMLNode::loadAttributes( const char* pszAttrs , LPPARSEINFO pi /*= &piDefault*/)
{
	char* xml = (char*)pszAttrs;

	while( xml && *xml )
	{
		if( xml = _tcsskip( xml ) )
		{
			if ( xml = _tctskip(xml) )
			{
				if ( xml = _tcrskip(xml) )
				{
					// close tag
					if( *xml == chXMLTagClose || *xml == chXMLTagPre )
						// wel-formed tag
						return xml;

					// XML Attr Name
					char* pEnd = _tcspbrk( xml, " =" );
					if( pEnd == NULL ) 
					{
						// error
						if( pi->erorr_occur == false ) 
						{
							pi->erorr_occur = true;
							pi->error_pointer = xml;
							pi->error_code = PIE_ATTR_NO_VALUE;
							pi->error_string = "<%s> attribute has error ";
							pi->error_string += name;
						}
						return xml;
					}

					LPXAttr attr = new XAttr;
					attr->parent = this;

					// XML Attr Name
					_Setstring( xml, pEnd, &attr->name );

					// add new attribute
					attrs.push_back( attr );
					xml = pEnd;

					// XML Attr Value
					if( xml = _tcsskip( xml ) )
					{
						//if( xml = _tcschr( xml, '=' ) )
						if( *xml == '=' )
						{
							if( xml = _tcsskip( ++xml ) )
							{
								// if " or '
								// or none quote
								int quote = *xml;
								if( quote == '"' || quote == '\'' )
									pEnd = _tcsechr( ++xml, quote, chXMLEscape );
								else
								{
									//attr= value> 
									// none quote mode
									//pEnd = _tcsechr( xml, ' ', '\\' );
									pEnd = _tcsepbrk( xml, " >", chXMLEscape );
								}

								bool trim = pi->trim_value;
								_Setstring( xml, pEnd, &attr->value, trim, chXMLEscape );	
								xml = pEnd;
								// ATTRVALUE 
								if( pi->entity_value && pi->entitys )
									attr->value = pi->entitys->ref2Entity(attr->value.c_str());

								if( quote == '"' || quote == '\'' )
									xml++;
							}
						}
					}
				}
			}
		}
	}

	// not wel-formed tag
	return NULL;
}

// <TAG attr1="value1" attr2='value2' attr3=value3 >
// </TAG>
// or
// <TAG />
//        ^- return pointer
//========================================================
// Desc   : load xml plain text
// Param  : pszXml - plain xml text
//          pi = parser information
// Return : advanced std::string pointer 
//========================================================
char* _tagXMLNode::load( const char* pszXml, LPPARSEINFO pi /*= &piDefault*/ )
{
	char* xml = (char*)pszXml;

	// initilize 
	parent = NULL;
	childs.clear();
	attrs.clear();

	xml = _tcschr( xml, chXMLTagOpen );
	if( xml == NULL )
		return xml;

	// Close Tag
	if( *(xml+1) == chXMLTagPre ) // </Close
		return xml;

	// XML Node Tag Name Open
	xml++;
	char* pTagEnd = _tcspbrk( xml, " />" );
	_Setstring( xml, pTagEnd, &name );
	xml = pTagEnd;
	// Generate XML Attributte List
	if( xml = loadAttributes( xml, pi ) )
	{
		// alone tag <TAG ... />
		if( *xml == chXMLTagPre )
		{
			xml++;
			if( *xml == chXMLTagClose )
				// wel-formed tag
				return ++xml;
			else
			{
				// error: <TAG ... / >
				if( pi->erorr_occur == false ) 
				{
					pi->erorr_occur = true;
					pi->error_pointer = xml;
					pi->error_code = PIE_ALONE_NOT_CLOSED;
					pi->error_string = "Element must be closed.";
				}
				// not wel-formed tag
				return xml;
			}
		}
		else
			// open/close tag <TAG ..> ... </TAG>
			//                        ^- current pointer
		{
			// insert if no text value
			if( this->value.empty() )
			{
				// Text Value 
				char* pEnd = _tcsechr( ++xml, chXMLTagOpen, chXMLEscape );
				if( pEnd == NULL ) 
				{
					if( pi->erorr_occur == false ) 
					{
						pi->erorr_occur = true;
						pi->error_pointer = xml;
						pi->error_code = PIE_NOT_CLOSED;
						pi->error_string = "%s must be closed with </%s>", name;
						pi->error_string += name;
					}
					// error cos not exist CloseTag </TAG>
					return xml;
				}

				bool trim = pi->trim_value;
				_Setstring( xml, pEnd, &value, trim, chXMLEscape );
				xml = pEnd;
				// TEXTVALUE reference
				if( pi->entity_value && pi->entitys )
					value = pi->entitys->ref2Entity(value.c_str());
			}

			// generate child nodes
			while( xml && *xml )
			{
				LPXNode node = new XNode;
				node->parent = this;

				xml = node->load( xml,pi );
				if( node->name.empty() == FALSE )
				{
					std::transform(	node->name.begin(), node->name.end(),	node->name.begin(), static_cast<int(*)(int)>(::tolower) );
					//node->name.MakeLower();
					childs.push_back( node );
				}
				else
				{
					delete node;
				}

				// open/close tag <TAG ..> ... </TAG>
				//                             ^- current pointer
				// CloseTag case
				if( xml && *xml && *(xml+1) && *xml == chXMLTagOpen && *(xml+1) == chXMLTagPre )
				{
					// </Close>
					xml+=2; // C

					if( xml = _tcsskip( xml ) )
					{
						std::string closename;
						char* pEnd = _tcspbrk( xml, " >" );
						if( pEnd == NULL ) 
						{
							if( pi->erorr_occur == false ) 
							{
								pi->erorr_occur = true;
								pi->error_pointer = xml;
								pi->error_code = PIE_NOT_CLOSED;
								pi->error_string = "it must be closed with";
								pi->error_string += name;
							}
							// error
							return xml;
						}
						_Setstring( xml, pEnd, &closename );
						if( closename == this->name )
						{
							// wel-formed open/close
							xml = pEnd+1;
							// return '>' or ' ' after pointer
							return xml;
						}
						else
						{
							xml = pEnd+1;
							// not welformed open/close
							if( pi->erorr_occur == false ) 
							{
								pi->erorr_occur = true;
								pi->error_pointer = xml;
								pi->error_code = PIE_NOT_NESTED;
								pi->error_string = name + "..." + closename + " is not wel-formed.";
							}
							return xml;
						}
					}
				}
				else	// Alone child Tag Loaded
				{

					if( xml && this->value.empty() && *xml !=chXMLTagOpen )
					{
						// Text Value 
						char* pEnd = _tcsechr( xml, chXMLTagOpen, chXMLEscape );
						if( pEnd == NULL ) 
						{
							// error cos not exist CloseTag </TAG>
							if( pi->erorr_occur == false )  
							{
								pi->erorr_occur = true;
								pi->error_pointer = xml;
								pi->error_code = PIE_NOT_CLOSED;
								pi->error_string = "it must be closed with"+ name;
							}
							return xml;
						}

						bool trim = pi->trim_value;
						_Setstring( xml, pEnd, &value, trim, chXMLEscape );
						xml = pEnd;
						//TEXTVALUE
						if( pi->entity_value && pi->entitys )
							value = pi->entitys->ref2Entity(value.c_str());
					}
				}
			}
		}
	}

	return xml;
}

//========================================================
// Desc   : convert plain xml text from parsed xml attirbute
// Return : converted plain std::string
//========================================================
std::string _tagXMLAttr::getXML( LPDISP_OPT opt /*= &optDefault*/ )
{
	std::ostringstream os;
	//os << (const char*)name << "='" << (const char*)value << "' ";
	os << name << "='"
		<< (opt->reference_value&&opt->entitys?opt->entitys->entity2Ref(value.c_str()):value) << "' ";
	return os.str();
}

//========================================================
// Desc   : convert plain xml text from parsed xml node
// Return : converted plain std::string
//========================================================
std::string _tagXMLNode::getXML( LPDISP_OPT opt /*= &optDefault*/ )
{
	std::ostringstream os;

	// tab
	if( opt && opt->newline )
	{
		if( opt && opt->newline )
			os << "\r\n";
		for( int i = 0 ; i < opt->tab_base ; i++)
			os << '\t';
	}

	// <TAG
	os << '<' << name;

	// <TAG Attr1="Val1" 
	if( attrs.empty() == false ) os << ' ';
	for( int i = 0 ; i < (int)attrs.size(); i++ )
	{
		os << attrs[i]->getXML(opt);
	}

	if( childs.empty() && value.empty() )
	{
		// <TAG Attr1="Val1"/> alone tag 
		os << "/>";	
	}
	else
	{
		// <TAG Attr1="Val1"> and get child
		os << '>';
		if( opt && opt->newline && !childs.empty() )
		{
			opt->tab_base++;
		}

		for( int i = 0 ; i < (int)childs.size(); i++ )
			os << childs[i]->getXML( opt );

		// Text Value
		if( value != "" )
		{
			if( opt && opt->newline && !childs.empty() )
			{
				if( opt && opt->newline )
					os << "\r\n";
				for( int i = 0 ; i < opt->tab_base ; i++)
					os << '\t';
			}
			os << (opt->reference_value&&opt->entitys?opt->entitys->entity2Ref(value.c_str()):value);
		}

		// </TAG> CloseTag
		if( opt && opt->newline && !childs.empty() )
		{
			os << "\r\n";
			for( int i = 0 ; i < opt->tab_base-1 ; i++)
				os << '\t';
		}
		os << "</" << name << '>';

		if( opt && opt->newline )
		{
			if( !childs.empty() )
				opt->tab_base--;
		}
	}

	return os.str();
}


//========================================================
// Name   : GetAttr
// Desc   : get attribute with attribute name
//========================================================
LPXAttr	_tagXMLNode::getAttr( const char* attrname )
{
	for( int i = 0 ; i < (int)attrs.size(); i++ )
	{
		LPXAttr attr = attrs[i];
		if( attr )
		{
			if( attr->name == attrname )
				return attr;
		}
	}
	return NULL;
}

//========================================================
// Name   : GetAttrs
// Desc   : find attributes with attribute name, return its list
//========================================================
XAttrs _tagXMLNode::getAttrs( const char* name )
{
	XAttrs attrs;
	for( int i = 0 ; i < (int)attrs.size(); i++ )
	{
		LPXAttr attr = attrs[i];
		if( attr )
		{
			if( attr->name == name )
				attrs.push_back( attr );
		}
	}
	return attrs;
}

//========================================================
// Name   : GetAttrValue
// Desc   : get attribute with attribute name, return its value
//========================================================
const char*	_tagXMLNode::getAttrValue( const char* attrname )
{
	LPXAttr attr = getAttr( attrname );
	return attr ? attr->value.c_str() : NULL;
}

//========================================================
// Name   : GetChilds
// Desc   : Find childs with name and return childs list
//========================================================
XNodes _tagXMLNode::getChilds( const char* name )
{
	XNodes nodes;
	for( int i = 0 ; i < (int)childs.size(); i++ )
	{
		LPXNode node = childs[i];
		if( node )
		{
			if( node->name == name )
				nodes.push_back( node );
		}
	}
	return nodes;	
}

XNodes _tagXMLNode::getChilds()
{
	return childs;
}

//========================================================
// Name   : GetChild
// Desc   : get child node with index
// Return : NULL return if no child.
//========================================================
LPXNode _tagXMLNode::getChild( int i )
{
	if( i >= 0 && i < (int)childs.size() )
		return childs[i];
	return NULL;
}

//========================================================
// Name   : GetChildCount
// Desc   : get child node count
// Return : 0 return if no child
//========================================================
int	_tagXMLNode::getChildCount()
{
	return (int)childs.size();
}

//========================================================
// Name   : GetChild
// Desc   : Find child with name and return child
// Return : NULL return if no child.
//========================================================
LPXNode	_tagXMLNode::getChild( const char* name )
{
	std::string strName = name;
	std::transform(	strName.begin(), strName.end(),	strName.begin(), static_cast<int(*)(int)>(::tolower) );
	//strName.MakeLower();

	for( int i = 0 ; i < (int)childs.size(); i++ )
	{
		LPXNode node = childs[i];
		if( node )
		{
			if( node->name == strName )
				return node;
		}
	}
	return NULL;
}

//========================================================
// Name   : GetChildValue
// Desc   : Find child with name and return child's value
// Return : NULL return if no child.
//========================================================
const char*	_tagXMLNode::getChildValue( const char* name )
{
	LPXNode node = getChild( name );
	return (node != NULL)? node->value.c_str() : NULL;
}

LPXAttr _tagXMLNode::getChildAttr( const char* name, const char* attrname )
{
	LPXNode node = getChild(name);
	return node ? node->getAttr(attrname) : NULL;
}

const char* _tagXMLNode::getChildAttrValue( const char* name, const char* attrname )
{
	LPXAttr attr = getChildAttr( name, attrname );
	return attr ? attr->value.c_str() : NULL;
}


//========================================================
// Name   : GetChildIterator
// Desc   : get child nodes iterator
// Return : NULL return if no childs.
//========================================================
XNodes::iterator _tagXMLNode::getChildIterator( LPXNode node )
{
	XNodes::iterator it = childs.begin();
	for( ; it != childs.end() ; ++(it) )
	{
		if( *it == node )
			return it;
	}
        return XNodes::end();
}

//========================================================
// Name   : AppendChild
// Desc   : add node
//========================================================
LPXNode	_tagXMLNode::appendChild( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
	return appendChild( createNode( name, value ) );
}

//========================================================
// Name   : AppendChild
// Desc   : add node
//========================================================
LPXNode _tagXMLNode::appendChild( LPXNode node )
{
	node->parent = this;
	childs.push_back( node );
	return node;
}

//========================================================
// Name   : RemoveChild
// Desc   : detach node and delete object
//========================================================
bool _tagXMLNode::removeChild( LPXNode node )
{
	XNodes::iterator it = getChildIterator( node );
	if( (LPXNode)(*it) )
	{
		delete *it;
		childs.erase( it );
		return true;
	}
	return false;
}

//========================================================
// Name   : GetAttr
// Desc   : get attribute with index in attribute list
//========================================================
LPXAttr _tagXMLNode::getAttr( int i )
{
	if( i >= 0 && i < (int)attrs.size() )
		return attrs[i];
	return NULL;
}

//========================================================
// Name   : GetAttrIterator
// Desc   : get attribute iterator
// Return : std::vector<LPXAttr>::iterator
//========================================================
XAttrs::iterator _tagXMLNode::getAttrIterator( LPXAttr attr )
{
	XAttrs::iterator it = attrs.begin();
	for( ; it != attrs.end() ; ++(it) )
	{
		if( *it == attr )
			return it;
	}
	return NULL;
}

//========================================================
// Name   : AppendAttr
// Desc   : add attribute
//========================================================
LPXAttr _tagXMLNode::appendAttr( LPXAttr attr )
{
	attr->parent = this;
	attrs.push_back( attr );
	return attr;
}

//========================================================
// Name   : RemoveAttr
// Desc   : detach attribute and delete object
//========================================================
bool _tagXMLNode::removeAttr( LPXAttr attr )
{
	XAttrs::iterator it = getAttrIterator( attr );
	if( (LPXNode)(*it) )
	{
		delete *it;
		attrs.erase( it );
		return true;
	}
	return false;
}

//========================================================
// Name   : CreateNode
// Desc   : Create node object and return it
//========================================================
LPXNode _tagXMLNode::createNode( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
	LPXNode node = new XNode;
	node->name = name;
	node->value = value;
	return node;
}

int		_tagXMLNode::getAttrCount()
{
	return (int)attrs.size();
}
//========================================================
// Name   : CreateAttr
// Desc   : create Attribute object and return it
//========================================================
LPXAttr _tagXMLNode::createAttr( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
	LPXAttr attr = new XAttr;
	attr->name = name;
	attr->value = value;
	return attr;
}

//========================================================
// Name   : AppendAttr
// Desc   : add attribute
//========================================================
LPXAttr _tagXMLNode::appendAttr( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
	return appendAttr( createAttr( name, value ) );
}

//========================================================
// Name   : DetachChild
// Desc   : no delete object, just detach in list
//========================================================
LPXNode _tagXMLNode::detachChild( LPXNode node )
{
	XNodes::iterator it = getChildIterator( node );
	if( (LPXNode)(*it) )
	{
		childs.erase( it );
		return node;
	}
	return NULL;
}

//========================================================
// Name   : DetachAttr
// Desc   : no delete object, just detach in list
//========================================================
LPXAttr _tagXMLNode::detachAttr( LPXAttr attr )
{
	XAttrs::iterator it = getAttrIterator( attr );
	if( (LPXNode)(*it) )
	{
		attrs.erase( it );
		return attr;
	}
	return NULL;
}


_tagXMLEntitys::_tagXMLEntitys( LPXENTITY entities, int count )
{
	for( int i = 0; i < count; i++)
		push_back( entities[i] );
}

LPXENTITY _tagXMLEntitys::getEntity( int entity )
{
	for( int i = 0 ; i < (int)size(); i ++ )
	{
		if( at(i).entity == entity )
			return LPXENTITY(&at(i));
	}
	return NULL;
}

LPXENTITY _tagXMLEntitys::getEntity( char* entity )
{
	for( int i = 0 ; i < (int)size(); i ++ )
	{
		char* ref = (char*)at(i).ref;
		char* ps = entity;
		while( ref && *ref )
			if( *ref++ != *ps++ )
				break;
		if( ref && !*ref )	// found!
			return LPXENTITY(&at(i));
	}
	return NULL;
}

int _tagXMLEntitys::getEntityCount( const char* str )
{
	int nCount = 0;
	char* ps = (char*)str;
	while( ps && *ps )
		if( getEntity( *ps++ ) ) nCount ++;
	return nCount;
}

int _tagXMLEntitys::ref2Entity( const char* estr, char* str, int strlen )
{
	char* pes = (char*)estr;
	char* ps = str;
	char* ps_end = ps+strlen;
	while( pes && *pes && ps < ps_end )
	{
		LPXENTITY ent = getEntity( pes );
		if( ent )
		{
			// copy entity meanning char
			*ps = ent->entity;
			pes += ent->ref_len;
		}
		else
			*ps = *pes++;	// default character copy
		ps++;
	}
	*ps = '\0';

	// total copied characters
	return (int)(ps-str);	
}

int _tagXMLEntitys::entity2Ref( const char* str, char* estr, int estrlen )
{
	char* ps = (char*)str;
	char* pes = (char*)estr;
	char* pes_end = pes+estrlen;
	while( ps && *ps && pes < pes_end )
	{
		LPXENTITY ent = getEntity( *ps );
		if( ent )
		{
			// copy entity std::string
			char* ref = (char*)ent->ref;
			while( ref && *ref )
				*pes++ = *ref++;
		}
		else
			*pes++ = *ps;	// default character copy
		ps++;
	}
	*pes = '\0';

	// total copied characters
	return (int)(pes-estr);
}

std::string _tagXMLEntitys::ref2Entity( const char* estr )
{
	std::string ret;
	if( estr )
	{
		int len = (int)_tcslen(estr);
		//char* esbuf = es.GetBufferSetLength( len+1 );
		//if( esbuf )
		char* esbuf = new char[len+1];
		ref2Entity( estr, esbuf, len );

		ret.assign(esbuf);

		delete[] esbuf;
	}
	return ret;
}

std::string _tagXMLEntitys::entity2Ref( const char* str )
{
	std::string ret;
	if( str )
	{
		int nEntityCount = getEntityCount(str);
		if( nEntityCount == 0 )
			return std::string(str);
		int len = (int)_tcslen(str) + nEntityCount*10 ;
		//char* sbuf = s.GetBufferSetLength( len+1 );
		//if( sbuf )
		char* sbuf = new char[len+1];
		entity2Ref( str, sbuf, len );

		ret.assign( sbuf );

		delete[] sbuf;
	}
	return ret;
}

std::string XRef2Entity( const char* estr )
{
	return vfxXMLParser::entityDefault.ref2Entity( estr );
}

std::string XEntity2Ref( const char* str )
{
	return vfxXMLParser::entityDefault.entity2Ref( str );
}



vfxXMLParser::vfxXMLParser(void)
{
}

vfxXMLParser::~vfxXMLParser(void)
{
}