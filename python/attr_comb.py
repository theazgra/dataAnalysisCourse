from Table.DataTable import DataTable

def save_result(fileName, combinations):
    if combinations == None or len(combinations) <= 0:
        return
    with open(fileName, 'w') as logFile:
        rule = 0
        for pc in combinations:
            rule += 1
            logFile.write("Association rule: " + str(rule) + "\n")
            logFile.write(pc.print_match_statistics())            
            logFile.write("\n")            

def get_conf(c):
    return c.confidence

def main():
    dt = DataTable("../data/weather_nominal.csv", 4, "Yes")
    allCombs = dt.generate_all_combinations()
    dt.order_rules_by(lambda x: x.ruleCount(), False)
    print(len(allCombs))

    print("Saving normal output...")
    save_result("normal.txt", allCombs)

    


if __name__ == "__main__":
    main()
