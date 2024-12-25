
#include <sousflow.h>
#include <utest.h>
UTEST_MAIN();
using namespace Sousflow;
UTEST(sousflow, parse)
{
    TaskYml yml = parse("../test/sample.yml");
    ASSERT_EQ(yml.variables.size(), 2);
    ASSERT_EQ(yml.tasks.size(), 6);
    ASSERT_EQ(yml.groups.size(), 2);

    EXPECT_STREQ(yml.path.c_str(), "../test/sample.yml");

    EXPECT_STREQ(yml.variables["project_name"].c_str(), "sousflow");
    EXPECT_STREQ(yml.variables["version"].c_str(), "1.0.0");

    EXPECT_STREQ(yml.tasks["task1"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task1"].args.c_str(), "10 100 0");
    ASSERT_EQ(yml.tasks["task1"].env.size(), 2);
    EXPECT_STREQ(yml.tasks["task1"].env["HTTP_PROXY"].c_str(), "proxy.google.com:80");
    EXPECT_STREQ(yml.tasks["task1"].env["HTTPS_PROXY"].c_str(), "proxy.google.com:80");
    

    EXPECT_STREQ(yml.tasks["task2"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task2"].args.c_str(), "20 100 0");
    EXPECT_STREQ(yml.tasks["task2"].log.c_str(), "${project_name}_v${version}.log");

    EXPECT_STREQ(yml.tasks["task3"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task3"].args.c_str(), "30 100 0");
    EXPECT_STREQ(yml.tasks["task3"].log.c_str(), "${name}.log");

    EXPECT_STREQ(yml.tasks["task4"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task4"].args.c_str(), "40 100 0");
    EXPECT_STREQ(yml.tasks["task5"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task5"].args.c_str(), "50 100 0");
    EXPECT_STREQ(yml.tasks["task6"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task6"].args.c_str(), "60 100 0");

    ASSERT_EQ(yml.groups["group1"].size(), 2);
    ASSERT_EQ(yml.groups["group2"].size(), 3);

    EXPECT_STREQ(yml.groups["group1"][0][0].c_str(), "task1");
    EXPECT_STREQ(yml.groups["group1"][1][0].c_str(), "task2");
    EXPECT_STREQ(yml.groups["group1"][1][1].c_str(), "task3");

    EXPECT_STREQ(yml.groups["group2"][0][0].c_str(), "group1");
    EXPECT_STREQ(yml.groups["group2"][1][0].c_str(), "task4");
    EXPECT_STREQ(yml.groups["group2"][1][1].c_str(), "task5");
    EXPECT_STREQ(yml.groups["group2"][2][0].c_str(), "task6");

}

UTEST(sousflow, post_parse)
{
    TaskYml yml = parse("../test/sample.yml");
    yml = post_parse(yml);
    ASSERT_EQ(yml.variables.size(), 2);
    ASSERT_EQ(yml.tasks.size(), 6);
    ASSERT_EQ(yml.groups.size(), 2);

    EXPECT_STREQ(yml.path.c_str(), "../test/sample.yml");

    EXPECT_STREQ(yml.variables["project_name"].c_str(), "sousflow");
    EXPECT_STREQ(yml.variables["version"].c_str(), "1.0.0");

    EXPECT_STREQ(yml.tasks["task1"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task1"].args.c_str(), "10 100 0");
    ASSERT_EQ(yml.tasks["task1"].env.size(), 2);
    EXPECT_STREQ(yml.tasks["task1"].env["HTTP_PROXY"].c_str(), "proxy.google.com:80");
    EXPECT_STREQ(yml.tasks["task1"].env["HTTPS_PROXY"].c_str(), "proxy.google.com:80");
    

    EXPECT_STREQ(yml.tasks["task2"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task2"].args.c_str(), "20 100 0");
    EXPECT_STREQ(yml.tasks["task2"].log.c_str(), "sousflow_v1.0.0.log");

    EXPECT_STREQ(yml.tasks["task3"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task3"].args.c_str(), "30 100 0");
    EXPECT_STREQ(yml.tasks["task3"].log.c_str(), "task3.log");

    EXPECT_STREQ(yml.tasks["task4"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task4"].args.c_str(), "40 100 0");
    EXPECT_STREQ(yml.tasks["task5"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task5"].args.c_str(), "50 100 0");
    EXPECT_STREQ(yml.tasks["task6"].cmd.c_str(), "task");
    EXPECT_STREQ(yml.tasks["task6"].args.c_str(), "60 100 0");

    ASSERT_EQ(yml.groups["group1"].size(), 2);
    ASSERT_EQ(yml.groups["group2"].size(), 3);

    EXPECT_STREQ(yml.groups["group1"][0][0].c_str(), "task1");
    EXPECT_STREQ(yml.groups["group1"][1][0].c_str(), "task2");
    EXPECT_STREQ(yml.groups["group1"][1][1].c_str(), "task3");

    EXPECT_STREQ(yml.groups["group2"][0][0].c_str(), "group1");
    EXPECT_STREQ(yml.groups["group2"][1][0].c_str(), "task4");
    EXPECT_STREQ(yml.groups["group2"][1][1].c_str(), "task5");
    EXPECT_STREQ(yml.groups["group2"][2][0].c_str(), "task6");

}