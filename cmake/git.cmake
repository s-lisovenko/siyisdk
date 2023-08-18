find_package(Git REQUIRED)

# Get commit hash
# GIT_COMMIT_HASH - output variable
# Example usage: git_get_commit_hash(GIT_COMMIT_HASH)
function(git_get_commit_hash GIT_COMMIT_HASH)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" log -1 --format=%h
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(${GIT_COMMIT_HASH} ${COMMIT_HASH} PARENT_SCOPE)

endfunction(git_get_commit_hash)

# Parse git tag and extract release candidate version
# Example of tag: v1.3.0-rc2
# GIT_RC_VERSION - output variable
# Example usage: get_rc_version(${GIT_TAG} GIT_RC_VERSION)
function(get_rc_version GIT_TAG GIT_RC_VERSION)
    string(REGEX MATCH "rc[0-9]+" TMP ${GIT_TAG})
    if("${TMP}" STREQUAL "")
        set(${GIT_RC_VERSION} ${RC_VERSION} PARENT_SCOPE)
    else()
        string(REGEX MATCH "[0-9]+" RC_VERSION ${TMP})
        set(${GIT_RC_VERSION} "-${RC_VERSION}" PARENT_SCOPE)
    endif()

endfunction(get_rc_version)

# Parse git tag and get git tag, major version and minor version
# If not tag on repo - set default tag 1.0.0
# Tag must contain version: v-1.0.1, release-1.8.1 ...
# GIT_TAG, GIT_TAG_VERSION_MAJOR, GIT_TAG_VERSION_MINOR GIT_TAG_VERSION_PATCH - output variables
# Example usage: git_get_latest_tag(GIT_TAG GIT_TAG_VERSION_MAJOR GIT_TAG_VERSION_MINOR GIT_TAG_VERSION_PATCH)
function(git_get_latest_tag GIT_TAG GIT_TAG_VERSION_MAJOR GIT_TAG_VERSION_MINOR GIT_TAG_VERSION_PATCH)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --abbrev=0
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE TAG_FROM_GIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE EXEC_STATUS
    )

    ##
    ## Parse tag
    ##
    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" TMP ${TAG_FROM_GIT})
    string(REGEX MATCHALL "[0-9]+" VERSION_LIST ${TMP})
    list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)

    if (${VERSION_LIST_LENGTH} LESS 3)
        message(SEND_ERROR "Failed to parge git tag major and minor version.")
    endif ()

    list(GET VERSION_LIST 0 VERSION_MAJOR)
    list(GET VERSION_LIST 1 VERSION_MINOR)
    list(GET VERSION_LIST 2 VERSION_PATCH)

    set(${GIT_TAG} ${TAG_FROM_GIT} PARENT_SCOPE)
    set(${GIT_TAG_VERSION_MAJOR} ${VERSION_MAJOR} PARENT_SCOPE)
    set(${GIT_TAG_VERSION_MINOR} ${VERSION_MINOR} PARENT_SCOPE)
    set(${GIT_TAG_VERSION_PATCH} ${VERSION_PATCH} PARENT_SCOPE)
endfunction(git_get_latest_tag)

# Get count commit from last tag
# GIT_NUMBER_OF_COMMITS_SINCE_TAG - output variable
# GIT_TAG - input variable contain git tag
# Example usage: git_get_number_of_commits_since_tag(GIT_COMMITS_SINCE_TAG ${GIT_TAG})
function(git_get_number_of_commits_since_tag GIT_NUMBER_OF_COMMITS_SINCE_TAG GIT_TAG)
    if(NOT ${GIT_TAG} STREQUAL "")
        set(COUNT_TO "...${GIT_TAG}")
    endif()

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-list --no-merges --count HEAD${COUNT_TO}
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE NUMBER_OF_COMMITS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE EXEC_STATUS
    )

    if (EXEC_STATUS EQUAL "0")
        set(${GIT_NUMBER_OF_COMMITS_SINCE_TAG} ${NUMBER_OF_COMMITS} PARENT_SCOPE)
    else ()
        set(${GIT_NUMBER_OF_COMMITS_SINCE_TAG} "0" PARENT_SCOPE)
    endif ()

endfunction(git_get_number_of_commits_since_tag)

# Sets GIT_TAG_EXISTS to TRUE if git tag command list some tags
# GIT_TAG_EXISTS - TRUE if git tag lists any tags
# Example usage: git_check_tags_exists(GIT_TAG_EXISTS)
function(git_check_tags_exists GIT_TAG_EXISTS)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_QUIET
        ERROR_QUIET
        RESULT_VARIABLE ERROR_CODE
    )

    if (${ERROR_CODE} EQUAL 0)
        set(${GIT_TAG_EXISTS} TRUE PARENT_SCOPE)
    else()
        set(${GIT_TAG_EXISTS} FALSE PARENT_SCOPE)
    endif()
endfunction(git_check_tags_exists)

# Get version components based on git tag.
# Please not that it works with annotated tags only.
# If no tags are visible, return version 1.0 and overall number of commits
# GIT_TAG_VERSION_MAJOR - major part of version
# GIT_TAG_VERSION_MINOR - minor part of version
# GIT_VERSION_SUFFIX - number commits since tag or overall
# Example usage: git_get_version(GIT_TAG_VERSION_MAJOR GIT_TAG_VERSION_MINOR GIT_TAG_VERSION_PATCH GIT_VERSION_SUFFIX)
function(git_get_version GIT_TAG_VERSION_MAJOR GIT_TAG_VERSION_MINOR GIT_TAG_VERSION_PATCH GIT_VERSION_SUFFIX)
    git_check_tags_exists(TAG_EXISTS)

    if (${TAG_EXISTS})
        git_get_latest_tag(TAG VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
        git_get_number_of_commits_since_tag(COMMITS_SINCE_TAG ${TAG})

        message(STATUS "git: Most recent visible tag: " ${TAG})
        message(STATUS "git: Number of commits: " ${COMMITS_SINCE_TAG})
    else()
        git_get_number_of_commits_since_tag(COMMITS_SINCE_TAG "")

        set(VERSION_MAJOR "1")
        set(VERSION_MINOR "0")
        set(VERSION_PATCH "0")

        message(STATUS "git: No git tag is visible. Will assume version 1.0.0")
        message(STATUS "git: Overall number of commits: " ${COMMITS_SINCE_TAG})
    endif()

    git_get_commit_hash(COMMIT_HASH)
    message(STATUS "git: Latest commit hash: " ${COMMIT_HASH})

    set(${GIT_TAG_VERSION_MAJOR} ${VERSION_MAJOR} PARENT_SCOPE)
    set(${GIT_TAG_VERSION_MINOR} ${VERSION_MINOR} PARENT_SCOPE)
    set(${GIT_TAG_VERSION_PATCH} ${VERSION_PATCH} PARENT_SCOPE)

    if(${COMMITS_SINCE_TAG} EQUAL 0)
      get_rc_version(${TAG} GIT_RC_VERSION)
      set(${GIT_VERSION_SUFFIX} ${GIT_RC_VERSION} PARENT_SCOPE)
    else()
      set(${GIT_VERSION_SUFFIX} ".${COMMITS_SINCE_TAG}-${COMMIT_HASH}" PARENT_SCOPE)
    endif()
endfunction(git_get_version)
