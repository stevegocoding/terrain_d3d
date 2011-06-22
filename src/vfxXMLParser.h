#ifndef __vfxXMLParser__H__
#define __vfxXMLParser__H__


#include "Prerequisites.h"


#pragma warning (push)

struct _tagXMLAttr;
typedef _tagXMLAttr XAttr, *LPXAttr;
typedef std::vector<LPXAttr> XAttrs;
struct _tagXMLNode;
typedef _tagXMLNode XNode, *LPXNode;
typedef std::vector<LPXNode> XNodes, *LPXNodes;
struct _tagParseInfo;

class vfxXMLParser
{
public:
	static struct _tagXMLEntitys entityDefault;
	static struct _tagParseInfo piDefault;
	static struct _tagDispOption optDefault;
public:
	vfxXMLParser(void);
	~vfxXMLParser(void);
};

/** Entity Encode/Decode Support
*/
typedef struct  _tagXmlEntity
{
	char entity;					// entity ( & " ' < > )
	char ref[10];					// entity reference ( &amp; &quot; etc )
	int ref_len;					// entity reference length
}XENTITY,*LPXENTITY;

typedef std::vector<struct _tagXmlEntity> XENTITYVector;

typedef struct _tagXMLEntitys : public XENTITYVector
{
	LPXENTITY getEntity( int entity );
	LPXENTITY getEntity( char* entity );	
	int getEntityCount( const char* str );
	int ref2Entity( const char* estr, char* str, int strlen );
	int entity2Ref( const char* str, char* estr, int estrlen );
	std::string ref2Entity( const char* estr );
	std::string entity2Ref( const char* str );	

	_tagXMLEntitys(){};
	_tagXMLEntitys( LPXENTITY entities, int count );
}XENTITYS,*LPXENTITYS;

//extern XENTITYS entityDefault;
std::string XRef2Entity( const char* estr );
std::string XEntity2Ref( const char* str );	

typedef enum 
{
	PIE_PARSE_WELFORMED	= 0,
	PIE_ALONE_NOT_CLOSED,
	PIE_NOT_CLOSED,
	PIE_NOT_NESTED,
	PIE_ATTR_NO_VALUE
}PCODE;

// Parse info.
typedef struct _tagParseInfo
{
	bool		trim_value;			// [set] do trim when parse?
	bool		entity_value;		// [set] do convert from reference to entity? ( &lt; -> < )
	LPXENTITYS	entitys;			// [set] entity table for entity decode

	char*		xml;				// [get] xml source
	bool		erorr_occur;		// [get] is occurance of error?
	char*		error_pointer;		// [get] error position of xml source
	PCODE		error_code;			// [get] error code
	std::string		error_string;		// [get] error std::string

	_tagParseInfo() { trim_value = false; entity_value = true; entitys = &vfxXMLParser::entityDefault; xml = NULL; erorr_occur = false; error_pointer = NULL; error_code = PIE_PARSE_WELFORMED; }
}PARSEINFO,*LPPARSEINFO;
//extern PARSEINFO piDefault;

// display optional environment
typedef struct _tagDispOption
{
	bool newline;			// newline when new tag
	bool reference_value;	// do convert from entity to reference ( < -> &lt; )
	LPXENTITYS	entitys;	// entity table for entity encode

	int tab_base;			// internal usage
	_tagDispOption() { newline = true; reference_value = true; entitys = &vfxXMLParser::entityDefault; tab_base = 0; }
}DISP_OPT, *LPDISP_OPT;
//extern DISP_OPT optDefault;

// XAttr : Attribute Implementation
typedef struct _tagXMLAttr
{
	std::string name;
	std::string	value;

	_tagXMLNode*	parent;

	std::string getXML( LPDISP_OPT opt = &vfxXMLParser::optDefault );
}XAttr, *LPXAttr;

// XMLNode structure
struct _tagXMLNode
{
	// name and value
	std::string name;
	std::string	value;

	// internal variables
	LPXNode	parent;		// parent node
	XNodes	childs;		// child node
	XAttrs	attrs;		// attributes

	// Load/Save XML
	char*	load( const char* pszXml, LPPARSEINFO pi = &vfxXMLParser::piDefault );
	char*	loadAttributes( const char* pszAttrs, LPPARSEINFO pi = &vfxXMLParser::piDefault );
	std::string getXML( LPDISP_OPT opt = &vfxXMLParser::optDefault );

	// in own attribute list
	LPXAttr	getAttr( const char* attrname ); 
	const char*	getAttrValue( const char* attrname ); 
	XAttrs	getAttrs( const char* name ); 

	// in one level child nodes
	LPXNode	getChild( const char* name ); 
	const char*	getChildValue( const char* name ); 
	XNodes	getChilds( const char* name ); 
	XNodes	getChilds(); 

	LPXAttr getChildAttr( const char* name, const char* attrname );
	const char* getChildAttrValue( const char* name, const char* attrname );

	// modify DOM 
	int	getChildCount();
	LPXNode getChild( int i );
	XNodes::iterator getChildIterator( LPXNode node );
	LPXNode createNode( const char* name = NULL, const char* value = NULL );
	LPXNode	appendChild( const char* name = NULL, const char* value = NULL );
	LPXNode	appendChild( LPXNode node );
	bool	removeChild( LPXNode node );
	LPXNode detachChild( LPXNode node );

	int		getAttrCount();
	LPXAttr getAttr( int i );
	XAttrs::iterator getAttrIterator( LPXAttr node );
	LPXAttr createAttr( const char* anem = NULL, const char* value = NULL );
	LPXAttr appendAttr( const char* name = NULL, const char* value = NULL );
	LPXAttr	appendAttr( LPXAttr attr );
	bool	removeAttr( LPXAttr attr );
	LPXAttr detachAttr( LPXAttr attr );

	// operator overloads
	LPXNode operator [] ( int i ) { return getChild(i); }

	_tagXMLNode() { parent = NULL; }
	~_tagXMLNode();

};

typedef struct _tagXMLNode XNode;
typedef struct _tagXMLNode *LPXNode;

 

// Helper Funtion
inline long XStr2Int( const char* str, long default_value = 0 )
{
	return str ? atol(str) : default_value;
}

#pragma warning (pop)


#endif