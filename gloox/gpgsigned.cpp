/*
  Copyright (c) 2006-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "gpgsigned.h"
#include "tag.h"

namespace gloox
{

  GPGSigned::GPGSigned( const std::string& signature )
    : StanzaExtension( ExtGPGSigned ),
      m_signature( signature ), m_valid( true )
  {
    if( m_signature.empty() )
      m_valid = false;
  }

  GPGSigned::GPGSigned( const Tag* tag )
    : StanzaExtension( ExtGPGSigned ),
      m_valid( false )
  {
    if( tag && tag->name() == "x" && tag->hasAttribute( XMLNS, XMLNS_X_GPGSIGNED ) )
    {
      m_valid = true;
      m_signature = tag->cdata();
    }
  }

  GPGSigned::~GPGSigned()
  {
  }

  const std::string& GPGSigned::filterString() const
  {
    static const std::string filter =
           "/presence/x[@xmlns='" + XMLNS_X_GPGSIGNED + "']"
           "|/message/x[@xmlns='" + XMLNS_X_GPGSIGNED + "']";
    return filter;
  }

  Tag* GPGSigned::tag() const
  {
    if( !m_valid )
      return 0;

    Tag* x = new Tag( "x", m_signature );
    x->addAttribute( XMLNS, XMLNS_X_GPGSIGNED );

    return x;
  }

}
