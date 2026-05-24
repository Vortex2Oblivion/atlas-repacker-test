#include "FileUtil.hpp"

FileResult FileUtil::openFileDialog(const std::vector<nfdu8filteritem_t>& filterList, const std::string& defaultPath) {
	auto* filters = static_cast<nfdu8filteritem_t *>(malloc(filterList.size() * sizeof(nfdu8filteritem_t)));

	for (size_t i = 0; i < filterList.size(); i++) {
		filters[i] = filterList[i];
	}

	const nfdopendialogu8args_t args = {
		.filterList = filters,
		.filterCount = static_cast<nfdfiltersize_t>(filterList.size()),
		.defaultPath = defaultPath.c_str(),
	};

	nfdu8char_t *outPath = nullptr;

	const auto result = NFD_OpenDialogU8_With(&outPath, &args);
	FileResult ret;

	if (result == NFD_OKAY) {
		ret = FileResult{.outPath = outPath, .result = result};
		NFD_FreePathU8(outPath);
	}
	free(filters);

	return ret;
}
