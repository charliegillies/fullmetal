/*
 * Simple type system for our scene graph.
 */

#pragma once

#include "fullmetal-config.h"

#include <map>
#include <vector>
#include <cassert>
#include <string>
#include <functional>
#include <typeindex>
#include "json.hpp"

namespace fm {
	class SceneNode;
	class NodeTypeTable;

	/*
	 * Creates a node type table
	 */
	NodeTypeTable* createDefaultTypeTable();

	template<class TNode>
	struct NodeFunctions {
		// typedef of a function that takes json/tnode params
		typedef std::function<void(nlohmann::json&, TNode& node)> ParseFunction;
		typedef std::function<void(TNode*)> IntrospectFunction;

		/* 
		 * Function that takes json & tnode params.
		 * Reads the node from JSON.
		 */
		ParseFunction readFunction;

		/* 
		 * Function that takes json & tnode params.
		 * Writes the node into JSON.
		 */
		ParseFunction writeFunction;

		/*
		 * Function that draws the internal settings of the TNode.
		 */
		IntrospectFunction introspectFunction;

		void set_parse_functions(ParseFunction readFunc, ParseFunction writeFunc) {
			readFunction = readFunc;
			writeFunction = writeFunc;
		}

		void set_introspection_function(IntrospectFunction introFunc) {
			introspectFunction = introFunc;
		}
	};

	/*
	 * The node type table is responsible for two big jobs:
	 *	1. Linking a string id of a scene node to the templates compile type.
	 *	2. Providing the GUI with a list of all possible scene nodes.
	 */
	class NodeTypeTable {
	private:
		class INodeTypeLink {
		public:
			std::string parse_id;

			virtual SceneNode* create_node() = 0;
			virtual SceneNode* read(nlohmann::json& json) = 0;
			virtual void write(nlohmann::json& json, SceneNode* node) = 0;
			virtual void introspect(SceneNode* node) = 0;
		};

		template<class TNode>
		class NodeTypeLink : public INodeTypeLink {
		public:
			NodeFunctions<TNode> nodeFunctions;

			virtual SceneNode* create_node() override {
				// Create the node with an empty constructor
				return new TNode();
			}

			virtual SceneNode* read(nlohmann::json & json) override {
				// ensure we have a read function for this node
				assert(nodeFunctions.readFunction);

				// read the node from JSON 
				TNode node;
				nodeFunctions.readFunction(json, node);

				TNode* node_ptr = new TNode(node);
				return node_ptr;
			}

			virtual void write(nlohmann::json & json, SceneNode* node) override {
				// ensure we have a write function for this node
				assert(nodeFunctions.writeFunction);

				// write the node to JSON
				auto castedNode = dynamic_cast<TNode*>(node);
				assert(castedNode != nullptr);
				nodeFunctions.writeFunction(json, *castedNode);
			}

			virtual void introspect(SceneNode * node) override
			{
				// write the node to JSON
				auto castedNode = dynamic_cast<TNode*>(node);
				assert(castedNode != nullptr);
				nodeFunctions.introspectFunction(castedNode);
			}
		};

		// string -> INodeTypeLink ptr
		std::map<std::string, INodeTypeLink*> _linksById;
		// type -> INodeTypeLink ptr
		std::map<std::type_index, INodeTypeLink*> _linksByType;

	public:
		~NodeTypeTable() {
			for (auto link : _linksById)
				delete link.second;
		}

		/*
		 * Registers a type to the type table.
		 *
		 * Param 'name' is used as a readable name of the TNode type,
		 * this is what will show when types are iterated over
		 * and what will be used for fullmetal-io for parsing objects from JSON.
		 */
		template<class TNode>
		NodeFunctions<TNode>& registerNode(std::string name) {
			// Assert that we don't already have a node registered with this name
			assert(_linksById[name] == nullptr);

			// get the type index from our TNode type
			std::type_index index = std::type_index(typeid(TNode));

			// create a node type link with our TNode type
			auto nodeLink = new NodeTypeLink<TNode>{};
			nodeLink->parse_id = name;
			_linksById[name] = nodeLink;
			_linksByType[index] = nodeLink;

			return nodeLink->nodeFunctions;
		}

		/*
		 * Attempts to write the node into json.
		 * Returns false if not possible, true if complete.
		 */
		bool writeNode(nlohmann::json& j, SceneNode* node) {
			// get the index from the node type
			std::type_index index = std::type_index(typeid(*node));
			// now try to get the node type link from the index
			auto nodeLink = _linksByType[index];
			if (nodeLink == nullptr) 
				return false;
			
			//TODO, some sort of check to ensure the write worked..
			nodeLink->write(j, node);

			// write the parse id so we know what object to parse on readNode()
			j["node_id"] = nodeLink->parse_id;

			return true;
		}

		void introspect(SceneNode* node) {
			std::type_index index = std::type_index(typeid(*node));
			// now try to get the node type link from the index
			auto nodeLink = _linksByType[index];
			if (nodeLink == nullptr) return;
			
			// introspect the node..
			nodeLink->introspect(node);
		}

		/* 
		 */
		SceneNode* readNode(nlohmann::json& j) {
			// try get the parse id from the jObj
			nlohmann::json jParseId = j["node_id"];
			assert(!jParseId.is_null());

			std::string parse_id = jParseId;
			auto nodeLink = _linksById[parse_id];
			assert(nodeLink != nullptr);

			// read the json to create the node..
			return nodeLink->read(j);
		}

		/*
		 * Gets a vector of the string ids registered to nodes in the type table.
		 */
		std::vector<std::string> getIds();

		/*
		 * Creates a scene node
		 */
		SceneNode* createNodeFromId(std::string id);
	};
}