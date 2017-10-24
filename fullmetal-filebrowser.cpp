#include "fullmetal-filebrowser.h"

#include <cassert>
#include <filesystem>
namespace fs = std::experimental::filesystem;

// Directory browse implementation
fm::DirectoryBrowseResult::DirectoryBrowseResult(const std::string & dirPath) : dirPath(dirPath) { }

fm::DirectoryBrowseResult::~DirectoryBrowseResult() {
	for (auto i : items)
		delete i;

	items.clear();
}

fm::DirectoryBrowseResult::Item::~Item() {
	for (auto i : children)
		delete i;

	children.clear();
}

// Recursive function for directory iteration
void iterateDirectory(std::vector<fm::DirectoryBrowseResult::Item*>& items, fs::directory_iterator& it);

fm::DirectoryBrowseResult * fm::browseDirectory(const std::string& directoryPath) {
	// Open a path to our directory, ensure the path exists.
	// if the path does not exist, return nullptr
	// and let the caller figure out what to do with it
	fs::path dirPath(directoryPath);
	if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
		return nullptr;
	}

	DirectoryBrowseResult* browseResult = new DirectoryBrowseResult(directoryPath);
	
	// iterate our directory for files and sub-directories
	auto iterator = fs::directory_iterator(dirPath);
	iterateDirectory(browseResult->items, iterator);

	return browseResult;
}

void iterateDirectory(std::vector<fm::DirectoryBrowseResult::Item*>& items, fs::directory_iterator & iterator) {
	// begin iteration of our directory items
	for (const auto& entry : iterator) {
		// create an item
		fm::DirectoryBrowseResult::Item* item = nullptr;

		if (fs::is_regular_file(entry)) {
			// declare file type item
			item = new fm::DirectoryBrowseResult::Item();
			item->type = fm::DirectoryBrowseResult::Item::FILE;

			item->os_filetype = entry.path().extension().generic_string();
		}
		else if (fs::is_directory(entry)) {
			// declare folder type item
			item = new fm::DirectoryBrowseResult::Item();
			item->type = fm::DirectoryBrowseResult::Item::FOLDER;

			item->os_filetype = ""; //none, as is folder.

			// iterate child directories on the folder, if any
			iterateDirectory(item->children, fs::directory_iterator(entry.path()));
		}

		if (item != nullptr) {
			item->fullpath = entry.path().generic_string();
			item->filename = entry.path().filename().generic_string();
			items.push_back(item);
		}
	}
}

#ifdef FM_EDITOR
#include "imgui\imgui.h"

// DIRECTORY GUI IMPLEMENTATION
fm::gui::DirectoryGuiView::DirectoryGuiView(const std::string & dirPath) {
	_result = browseDirectory(dirPath);
	assert(_result != nullptr);
}

fm::gui::DirectoryGuiView::DirectoryGuiView(DirectoryBrowseResult * result) {
	assert(result != nullptr);
	_result = result;
}

fm::gui::DirectoryGuiView::~DirectoryGuiView() {
	if (_result != nullptr)
		delete _result;
}

void fm::gui::DirectoryGuiView::refresh() {

}

bool fm::gui::DirectoryGuiView::itemAllowed(DirectoryBrowseResult::Item & item)
{
	// filetype conditions don't apply to folders
	if (item.type == item.FOLDER) return true;

	// if no conditions, return true
	if (_okFileTypes.size() == 0) return true;

	// otherwise, check the internal collection for this filetype
	if (std::find(_okFileTypes.begin(), _okFileTypes.end(), item.os_filetype) != _okFileTypes.end()) {
		return true;
	}

	return false;
}

fm::gui::DirectoryGuiView& fm::gui::DirectoryGuiView::setSelectCallback(std::function<void(std::string)> callback) {
	_selectedCallback = callback;
	return *this;
}

fm::gui::DirectoryGuiView & fm::gui::DirectoryGuiView::setAllowedFiletypes(std::vector<std::string>& filetypes)
{
	_okFileTypes = filetypes;
	return *this;
}

fm::gui::DirectoryGuiView & fm::gui::DirectoryGuiView::setAllowedFiletype(std::string & filetype)
{
	_okFileTypes.push_back(filetype);
	return *this;
}

void fm::gui::DirectoryGuiView::update() {
	// Push ID so we don't get conflicts
	ImGui::PushID("FILEBROWSER");

	// generated id for the imgui window
	std::string id = "Folder Browser##" + _result->dirPath;
	
	// open the window
	if (ImGui::Begin(id.c_str())) {
		// Draw the directory
		if (ImGui::BeginChild("FILEBROWSER-CHILD", ImVec2{}, true, ImGuiWindowFlags_HorizontalScrollbar)) {
			// Show the current viewed directory
			ImGui::Text(_result->dirPath.c_str());

			ImGui::Indent();
			
			// Draw all sub directory items, drawDirectory is recursive
			for (auto& i : _result->items) {
				drawDirectory(*i);
			}

			ImGui::Unindent();

			ImGui::EndChild();
		}


		ImGui::End();
	}

	ImGui::PopID();
}

void fm::gui::DirectoryGuiView::drawDirectory(DirectoryBrowseResult::Item & item) {

	if (!itemAllowed(item)) return;

	if (item.type == item.FILE) {
		std::string fileid = " " + item.filename;
		// Draws a selectable label, check for double click
		if (ImGui::Selectable(fileid.c_str()) && ImGui::IsMouseDoubleClicked) {
			// if we can invoke the callback, invoke it
			if (_selectedCallback) {
				_selectedCallback(item.fullpath);
			}
		}
	}
	else if (item.type == item.FOLDER) {
		// Draw the folder label
		std::string folderid = item.filename + "/";
		ImGui::Text(folderid.c_str());

		// Draw sub-directories, indent to show hierarchy
		ImGui::Indent();
		for (auto& i : item.children)
			drawDirectory(*i);
		ImGui::Unindent();
	}
}

#endif
