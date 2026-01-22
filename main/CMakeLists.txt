# idf_component_register(SRCS "main.c"
#                     INCLUDE_DIRS ".")
# idf_component_register(SRCS "dummy_main_for_tests.c"  # A minimal main if needed
idf_component_register(SRCS "main.c"  # A minimal main if needed
                       INCLUDE_DIRS "."
                       REQUIRES interchip_comm)
                    #    REQUIRES unity interchip_comm)