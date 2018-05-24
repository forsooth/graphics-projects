/*!
   @file   SceneParser.h
   @author Eric Tamura (October 2006)
   @author Nong Li (August 2007)
   @author Remco Chang (December 2013)
*/

#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "SceneData.hpp"
#include <string>
#include "SceneData.hpp"
#include "Algebra.hpp"
#include "tinyxml.hpp"
#include "tinystr.hpp"


#include <vector>
#include <map>

class TiXmlElement;


//! This class parses the scene graph specified by the CS123 Xml file format.
class SceneParser
{
   public:
      //! Create a parser, passing it the scene file.
      SceneParser(const std::string& filename);
      //! Clean up all data for the scene
      ~SceneParser();

      SceneParser& operator=(const SceneParser& other) {
        // check for self-assignment
        if(&other == this)
            return *this;
        // reuse storage when possible
         
         this->file_name = other.file_name;
         memset(&this->m_cameraData, 0, sizeof(SceneCameraData));
         memset(&this->m_globalData, 0, sizeof(SceneGlobalData));
         this->m_objects.clear();
         this->m_lights.clear();
         this->m_nodes.clear();

        return *this;
      }

      //! Parse the scene.  Returns false if scene is invalid.
      bool parse();

      //! Returns global scene data
      bool getGlobalData(SceneGlobalData& data) const;

      //! Returns camera data
      bool getCameraData(SceneCameraData& data) const;

      //! Returns the root scenegraph node
      SceneNode* getRootNode();

      //! Returns number of lights in the scene
      unsigned int getNumLights() const;

      //! Returns the ith light data
      bool getLightData(unsigned int i, SceneLightData& data) const;


   private:
      //the filename should be contained within this parser implementation
      //if you want to parse a new file, instantiate a different parser
      bool parseGlobalData(const TiXmlElement* scenefile);
      bool parseCameraData(const TiXmlElement* scenefile);
      bool parseLightData(const TiXmlElement* scenefile);
      bool parseObjectData(const TiXmlElement* scenefile);
      bool parseTransBlock(const TiXmlElement* transblock, SceneNode* node);
      bool parsePrimitive(const TiXmlElement* prim, SceneNode* node);
      
      std::string file_name;
      mutable std::map<std::string, SceneNode*> m_objects;
      SceneCameraData m_cameraData;
      std::vector<SceneLightData*> m_lights;
      SceneGlobalData m_globalData;
      std::vector<SceneNode*> m_nodes;
};

#endif

