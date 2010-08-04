// @HEADER
// ***********************************************************************
// 
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef TEUCHOS_VALIDATORXMLCONVERTER_HPP
#define TEUCHOS_VALIDATORXMLCONVERTER_HPP

/*! \file Teuchos_ValidatorXMLConverter.hpp
*/

#include "Teuchos_XMLObject.hpp"
#include "Teuchos_ValidatorMaps.hpp"
#include "Teuchos_Describable.hpp"
#include "Teuchos_XMLParameterListExceptions.hpp"


namespace Teuchos {



/** \brief An abstract base class for converting ParameterEntryValidators to
 * and from XML.
 */
class ValidatorXMLConverter : public Describable {
public:

  /** \brief Converts a given XMLObject to a ParameterEntryValidator.
   *
   * @param xmlObj The XMLObject to convert to a ParameterEntryValidator.
   * @return The converted ParameterEntryValidator.
   */
  RCP<ParameterEntryValidator>
  fromXMLtoValidator(const XMLObject& xmlObj, IDtoValidatorMap& validatorMap) const {
    #ifdef HAVE_TEUCHOS_DEBUG
	RCP<const ParameterEntryValidator> dummyValidator = getDummyValidator();
    TEST_FOR_EXCEPTION(xmlObj.getTag() != dummyValidator->getXMLTagName(), 
      BadValidatorXMLConverterException, 
      "Cannot convert xmlObject " << 
	  ". Expected a " << dummyValidator->getXMLTagName() <<
      " tag but got a " << xmlObj.getTag() << "tag");
    #endif
  
    RCP<ParameterEntryValidator> toReturn = convertXML(xmlObj, validatorMap);
    if(xmlObj.hasAttribute(getIdAttributeName())){

	  TEST_FOR_EXCEPTION(
		validatorMap.getValidator(xmlObj.getRequiredInt(getIdAttributeName()))
	    !=
		validatorMap.end(),
		DuplicateValidatorIDsException,
		"Two validators in the XML file have the same ID!" <<std::endl<<
		"Conflicting ID: " << xmlObj.getRequiredInt(getIdAttributeName()));
        
      validatorMap.insertValidator(
        IDtoValidatorMap::IDValidatorPair(
          xmlObj.getRequiredInt(getIdAttributeName()), toReturn));
    }
    return toReturn;
  }

  /** \brief Preforms any and all special xml conversion that is specific to a
   * particular ParameterEntryValidator.
   *
   * @param xmlObj The xml to be converted.
   *
   * @param validatorMap The validator map storing all validators that are
   * being converted.
   */
  virtual RCP<ParameterEntryValidator> convertXML(const XMLObject& xmlObj,
    IDtoValidatorMap& validatorMap) const=0;

  /** \brief Converters a given ParameterEntryValidator to XML.
   *
   * @param validator The ParameterEntryValidator to be converted to XML.
   *
   * @return An XML representation of the given ParameterEntryValidator.
   */
  XMLObject fromValidatortoXML(const RCP<const ParameterEntryValidator> validator,
    const ValidatortoIDMap& validatorMap) const
    {
      XMLObject toReturn = convertValidator(validator, validatorMap);
      ValidatortoIDMap::const_iterator result = validatorMap.getID(validator);
      if(result != validatorMap.end()){
        toReturn.addAttribute(getIdAttributeName(), result->second);
      }
      return toReturn;
    }


  // 2010/07/30: rabartl: ToDo: Move the above function bodies into the *.cpp
  // file!

  
  /** \brief Preforms any and all special validator conversion that is
   * specific to a particlar ParameterEntryValidator
   *
   * @param validator The validator to be converted.
   *
   * @param validatorMap The validator map storing all validators that are
   * being converted.
   */
  virtual XMLObject convertValidator(const RCP<const ParameterEntryValidator> validator,
    const ValidatortoIDMap& validatorMap) const = 0;

  #ifdef HAVE_TEUCHOS_DEBUG
  /**
   * \brief Returns the a default dummy validator.
   *
   * @return A default dummy validator.
   */
  virtual Teuchos::RCP<const ParameterEntryValidator> getDummyValidator() const = 0;
  #endif

  /** \brief . */
  static const std::string& getIdAttributeName(){
    static const std::string idAttributeName = "validatorid";
    return idAttributeName;
  }

  /** \brief . */
  static const std::string& getPrototypeIdAttributeName(){
    static const std::string prototypeIdAttributeName = "prototypeid";
    return prototypeIdAttributeName;
  }

};


} // namespace Teuchos


#endif // TEUCHOS_VALIDATORXMLCONVERTER_HPP
