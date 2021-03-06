
include_directories( ${Qt5Core_INCLUDE_DIRS} )

set( DATA_TEST_MODULE_HDRS      data/MyCustomType.hpp )

set( DATA_TEST_MODULE_MOC_HDRS  data/MyBlock.hpp
                                data/MyBlock1.hpp
                                data/MyBlock2.hpp
                                data/MyLibrary.hpp
                                data/MyModule.hpp )

set( DATA_TEST_MODULE_SRCS  data/MyBlock.cpp
                            data/MyBlock1.cpp
                            data/MyBlock2.cpp
                            data/MyCustomType.cpp
                            data/MyLibrary.cpp
                            data/MyModule.cpp )

qt5_wrap_cpp( DATA_TEST_MODULE_MOC_SRCS ${DATA_TEST_MODULE_MOC_HDRS} )

add_library( DataTestModule STATIC  ${DATA_TEST_MODULE_HDRS}
                                    ${DATA_TEST_MODULE_MOC_HDRS}
                                    ${DATA_TEST_MODULE_MOC_SRCS}
                                    ${DATA_TEST_MODULE_SRCS} )

target_link_libraries( DataTestModule ${KORE_TARGET} )

mpb_test_add( data test_main.cpp data/data_tests.cpp )
mpb_test_link_libraries( data ${KORE_TARGET} DataTestModule )

mpb_test_add( parallel test_main.cpp parallel/TestTasklet.cpp )
mpb_test_link_libraries( parallel ${KORE_TARGET} )

mpb_test_add( serialization test_main.cpp
                            serialization/serialization_tests.cpp )
mpb_test_link_libraries( serialization ${KORE_TARGET} DataTestModule )
