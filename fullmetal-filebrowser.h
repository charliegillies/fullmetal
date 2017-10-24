#pragma once

#include <string>
#include <vector>
#include <functional>
#include "fullmetal-config.h"

namespace fm {
	/*
	 * The result of a directory search.
	 */
	struct DirectoryBrowseResult {
		
		DirectoryBrowseResult(const std::string& dirPath);
		~DirectoryBrowseResult();

		/*
		 * An item in the directory.
		 * Can be a file or a folder.
		 */
		struct Item {
			~Item();

			enum Type { FILE = 1, FOLDER = 2 };
			
			/** If item is a file or folder. */
			Type type;

			/** The filetype according to the OS (.obj, .png, .jpg) */
			std::string os_filetype;

			/** The name of the file, e.g. ("picture.png") */
			std::string filename;

			/** Fullpath to the item from current directory search. */
			std::string fullpath;

			/** Child items - only contains data if 'type' is type directory. */
			std::vector<Item*> children;
		};

		/*
		 * All items from the file browse result.
		 */
		std::vector<Item*> items;

		/*
		 * The path that was searched.
		 */
		const std::string dirPath;
	};

	/*
	 * Returns a ptr instance of a directory browse result,
	 * which is a list of items that were found inside
	 * of a given directory path.
	 */
	DirectoryBrowseResult* browseDirectory(const std::string& directoryPath);

#ifdef FM_EDITOR
	namespace gui {
		/** Handles the drawing of a DirectoryBrowseResult. */
		class DirectoryGuiView {
		private:
			DirectoryBrowseResult* _result;
			std::function<void(std::string)> _selectedCallback;
			std::vector<std::string> _okFileTypes;

			/** Draw a directory. The function is recursive. */
			void drawDirectory(DirectoryBrowseResult::Item& item);

			/** Refreshes the directory, incase there's been a file change outside of the env. */
			void refresh();

			/** Checks the item against the _okFileTypes conditions. */
			bool itemAllowed(DirectoryBrowseResult::Item& item);

		public:
			DirectoryGuiView(const std::string& dirPath);
			DirectoryGuiView(DirectoryBrowseResult* result);
			~DirectoryGuiView();

			/** Sets the callback that is invoked on a double click. */
			DirectoryGuiView& setSelectCallback(std::function<void(std::string)> callback);

			/** Sets an internal collection of allowed filetypes. Overrides current allowed types. */
			DirectoryGuiView& setAllowedFiletypes(std::vector<std::string>& filetypes);

			/** Adds an allowed filetype to the collection.*/
			DirectoryGuiView& setAllowedFiletype(std::string& filetype);

			/** Sends the ImGui commands to draw the given directory. */
			void update();

		};
	}
#endif

}