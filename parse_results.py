# # SCRIPT ΕΞΑΓΩΓΗΣ ΜΕΤΡΙΚΩΝ ΑΠΟ OUTPUT FILES

# import os
# import pandas as pd
# import re
# import glob

# def parse_output(output_file):
#     """Εξάγει μετρικές από το output.dat"""

#     try:
#         with open(output_file, 'r', encoding='utf-8') as file:
#             content = file.read()

#         #Εύρεση αλγόριθμου
#         algorithm = re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content).group(1) if re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content) else "Unknown"

#         #Εξαγωγή μετρικών
#         metrics = {
#             'Algorithm' : algorithm,
#             'File' : os.path.basename(output_file),
#             'AF': extract_metric(r'Average AF: (\d+\.\d+)', content),
#             'Rec@ll' : extract_metric(r'Recall@N: (\d+\.\d+)', content), 
#             'QPS' : extract_metric(r'QPS: (\d+\.\d+)', content),
#             'Approximate_Time' : extract_metric(r'tApproximateAverage: (\d+\.\d+)', content),
#             'True_Average' : extract_metric(r'tTrueAverage: (\d+\.\d+)', content),
#             'Dataset': 'MNIST' if 'mnist' in output_file.lower() else 'SIFT'
#         }

#         #εξαγωγή παραμέτρων από το output_file
#         metrics.update(extract_params(output_file))
#         return metrics

#     except Exception as ex:
#         print(f"Error parsing file {output_file}: {ex}")
#         return None
    
# def extract_metric(pattern, content):
#     """Εξάγει την τιμή από το pattern που του δίνεται"""
#     return float(re.search(pattern, content).group(1)) if re.search(pattern, content) else 0.0

# def extract_params(output):
#     """Εξάγει παραμέτρους από το όνομα του αρχείου"""
    
#     params = {}
#     output_lower = output.lower()

#     #LSH params
#     if 'k' in output_lower and 'L' in output_lower:
#         k_match = re.search(r'k(\d+)', output_lower)
#         L_match = re.search(r'L(\d+)', output_lower)
#         w_match = re.search(r'w(\d+)', output_lower)

#         params.update({
#             'k' : int(k_match.group(1)) if k_match else 10,
#             'L' : int(L_match.group(1)) if L_match else 30,
#             'w' : float(w_match.group(1)) if w_match else 1.5
#         })

#     #Hypercube params
#     elif 'kproj' in output_lower or 'm' in output_lower:
#         kproj_match = re.search(r'kproj(\d+)', output_lower)
#         m_match = re.search(r'm(\d+)', output_lower)
#         probes_match = re.search(r'probes?(\d+)', output_lower)

#         params.update({
#             'kproj' : int(kproj_match.group(1)) if kproj_match else 8,
#             'm' : int(m_match.group(1)) if m_match else 100,
#             'probes' : int(probes_match.group(1)) if probes_match else 20
#         })

#     #IVFFlat params
#     elif 'kcluster' in output_lower:
#         kcluster_match = re.search(r'kcluster(\d+)', output_lower)
#         nprobe_match = re.search(r'nprobe?(\d+)', output_lower)

#         params.update({
#             'kcluster' : int(kcluster_match.group(1)) if kcluster_match else 256,
#             'nprobe' : int(nprobe_match.group(1)) if nprobe_match else 20
#         })

#     return params

# def main():
#     """Βασική συνάρτηση επεξεργασιαςαποτελεσματων"""

#     #Βρισκει όλα τα output files
#     output_files = glob.glob("*.dat")
#     all_metrics = []
#     for i in output_files:
#         # print(i + "\n")
#         metrics = parse_output(i)
#         if metrics:
#             all_metrics.append(metrics)
    
#     #create dataframe   
#     df = pd.DataFrame(all_metrics)

#     if df.empty:
#         print("No results found!")
#         return
    
#     #save in csv
#     df.to_csv('experiment_results.csv', index=False)    
#     print(f"saved results for {len(df)} experiments to experiment_results.csv")

#     #print results 
#     print("\nResults:\n")
#     print(df[['Algorithm', 'File', 'AF', 'Rec@ll', 'QPS', 'Approximate_Time', 'True_Average', 'Dataset']].head(10))

#     return df


# if __name__ == "__main__":
#     df = main()


# SCRIPT ΕΞΑΓΩΓΗΣ ΜΕΤΡΙΚΩΝ ΑΠΟ OUTPUT FILES - QUERY LEVEL

import os
import pandas as pd
import re
import glob

def parse_output(output_file):
    """Εξάγει μετρικές για ΚΑΘΕ query ξεχωριστά"""
    try:
        with open(output_file, 'r', encoding='utf-8') as file:
            content = file.read()

        # Εύρεση αλγόριθμου
        algorithm = re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content).group(1) if re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content) else "Unknown"

        # Διαχωρισμός σε queries
        queries = content.split('-------------------')
        all_query_metrics = []
        
        for i, query_block in enumerate(queries):
            if 'Query:' in query_block:
                # Εξαγωγή μετρικών για κάθε query
                query_metrics = {
                    'Algorithm': algorithm,
                    'File': os.path.basename(output_file),
                    'Query_ID': extract_query_id(query_block),
                    'AF': extract_metric(r'Average AF: (\d+\.\d+)', query_block),
                    'Recall': extract_metric(r'Recall@N: (\d+\.\d+)', query_block),
                    'QPS': extract_metric(r'QPS: (\d+\.\d+)', query_block),
                    'Approximate_Time': extract_metric(r'tApproximateAverage: (\d+\.\d+)', query_block),
                    'True_Time': extract_metric(r'tTrueAverage: (\d+\.\d+)', query_block),
                    'Dataset': 'MNIST' if 'mnist' in output_file.lower() else 'SIFT'
                }
                
                # Εξαγωγή παραμέτρων
                query_metrics.update(extract_params(output_file))
                all_query_metrics.append(query_metrics)
        
        print(f"✅ Found {len(all_query_metrics)} queries in {output_file}")
        return all_query_metrics

    except Exception as ex:
        print(f"Error parsing file {output_file}: {ex}")
        return None

def extract_query_id(query_block):
    """Εξάγει το ID του query"""
    match = re.search(r'Query: (\d+)', query_block)
    return int(match.group(1)) if match else 0

def extract_metric(pattern, content):
    """Εξάγει την τιμή από το pattern"""
    match = re.search(pattern, content)
    return float(match.group(1)) if match else 0.0

def extract_params(output):
    """Εξάγει παραμέτρους από το όνομα του αρχείου"""
    params = {}
    output_lower = output.lower()

    # LSH params
    if 'lsh' in output_lower:
        k_match = re.search(r'k(\d+)', output_lower)
        L_match = re.search(r'l(\d+)', output_lower)
        w_match = re.search(r'w(\d+)', output_lower)
        params.update({
            'k': int(k_match.group(1)) if k_match else 10,
            'L': int(L_match.group(1)) if L_match else 30,
            'w': float(w_match.group(1)) if w_match else 1.5
        })

    # Hypercube params
    elif 'hypercube' in output_lower or 'hyp' in output_lower:
        kproj_match = re.search(r'kproj(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        probes_match = re.search(r'probes?(\d+)', output_lower)
        params.update({
            'kproj': int(kproj_match.group(1)) if kproj_match else 14,
            'M': int(m_match.group(1)) if m_match else 10,
            'probes': int(probes_match.group(1)) if probes_match else 2
        })

    # IVFFlat params
    elif 'ivfflat' in output_lower or 'flat' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5
        })

    return params

def main():
    """Βασική συνάρτηση επεξεργασίας αποτελεσμάτων"""

    # Βρίσκει όλα τα output files
    output_files = glob.glob("*.dat")
    all_metrics = []
    
    for file in output_files:
        print(f"📖 Processing {file}...")
        query_metrics = parse_output(file)
        if query_metrics:
            all_metrics.extend(query_metrics)
    
    # Create dataframe   
    df = pd.DataFrame(all_metrics)

    if df.empty:
        print("No results found!")
        return
    
    # Save in csv
    df.to_csv('query_level_results.csv', index=False)    
    print(f"✅ Saved {len(df)} query-level results to query_level_results.csv")

    # Print results - Group by file to see all queries
    print("\n📊 QUERY-LEVEL RESULTS:")
    for file in df['File'].unique():
        file_data = df[df['File'] == file]
        print(f"\n📁 {file} ({len(file_data)} queries):")
        print(file_data[['Query_ID ', 'AF', 'Recall', 'QPS', 'Approximate_Time', 'True_Time']].to_string(index=False))

    return df

if __name__ == "__main__":
    df = main()