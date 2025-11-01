#SCRIPT ΕΞΑΓΩΓΗΣ ΜΕΤΡΙΚΩΝ ΑΠΟ OUTPUT FILES

import os               #βιβλιοθήκη για λειτουργίες αρχείων και φακέλων
import pandas as pd     #βιβλιοθήκη για χειρισμό δεδομένων και dataframe
import re               #βιβλιοθήκη για κανονικές εκφράσεις
import glob             #βιβλιοθήκη για αναζήτηση αρχείων με wildcards

#συνάρτηση που εξάγει μόνο τις τελικές μετρικές για κάθε εκτέλεση
def parse_final_metrics(output_file):
    try:
        #άνοιγμα και ανάγνωση του output αρχείου 
        with open(output_file, 'r', encoding='utf-8') as file:
            content = file.read()

        algorithm_match = re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content)  #εύρεση αλγόριθμου στο κείμενο χρησιμοποιώντας regex
        algorithm = algorithm_match.group(1) if algorithm_match else "Unknown"  #αντιστοίχιση του αλγορίθμου ή "Unknown" αν δεν βρεθεί

        #δημιουργία λεξικού με τις τελικές μετρικές 
        final_metrics = {
            'Algorithm': algorithm,                 #όνομα αλγορίθμου
            'File': os.path.basename(output_file),  #όνομα αρχείου χωρίς το path
            'Dataset': 'MNIST' if 'mnist' in output_file.lower() else 'SIFT',                           #αναγνώριση dataset
            'Avg_AF': extract_final_metric(r'Total Average AF: (\d+\.\d+)', content),                   #μέσος AF
            'Avg_Recall': extract_final_metric(r'Total Average Recall@N: (\d+\.\d+)', content),         #μέσος Recall@N
            'QPS': extract_final_metric(r'QPS: (\d+\.\d+)', content),                                   #QPS
            'Avg_Approx_Time': extract_final_metric(r'Average Approximate Time: (\d+\.\d+)', content),  #μέσος χρόνος προσέγγισης
            'Avg_Exact_Time': extract_final_metric(r'Average Exact Time: (\d+\.\d+)', content),         #μέσος ακριβής χρόνος
        }
        
        final_metrics.update(extract_params(output_file))       #προσθήκη των παραμέτρων στο λεξικό 
        
        print(f"Extracted final metrics from {output_file}")    #εκτύπωση μηνύματος επιβεβαίωσης
        return final_metrics

    except Exception as ex:
        print(f"Error parsing file {output_file}: {ex}")    #χειρισμός αρχείων κατά την ανάγνωση αρχείου
        return None


#συνάρτηση που εξάγει την τιμή από το pattern για τελικές μετρικές 
def extract_final_metric(pattern, content):
    match = re.search(pattern, content) #αναζήτη του pattern στο κείμενο
    
    if match:   #αν βρεθεί match, επεξεργασία και μετατροπή σε float
        value_str = match.group(1).rstrip('.')  #αφαιρεί τελεία στο τέλος αν υπάρχει
        return float(value_str)
    return 0.0  #επιστρέφει 0.0 αν δεν βρεθεί η τιμή 


#συνάρτηση που εξάγει παραμέτρους από το όνομα του αρχείου
def extract_params(output):
    params = {}                     #λεξικό για αποθήκευση παραμέτρων
    output_lower = output.lower()   #μετατροπή σε lowercase για case-insensitive αναζήτηση

    #εξαγωγή παραμέτρων για LSH αλγόριθμο
    if 'lsh' in output_lower:
        k_match = re.search(r'k(\d+)', output_lower)    
        L_match = re.search(r'l(\d+)', output_lower)
        w_match = re.search(r'w(\d+(?:\.\d+)?)', output_lower)
        
        #δίνει default τιμές αν δεν υπάρχουν οι παράμετροι
        params.update({
            'k': int(k_match.group(1)) if k_match else 4,
            'L': int(L_match.group(1)) if L_match else 5,
            'w': float(w_match.group(1)) if w_match else 4.0
        })

    #εξαγωγή παραμέτρων για Hypercube αλγόριθμο
    elif 'hypercube' in output_lower or 'hyp' in output_lower:
        kproj_match = re.search(r'kproj(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        probes_match = re.search(r'probes?(\d+)', output_lower)
        w_match = re.search(r'w(\d+(?:\.\d+)?)', output_lower)
        
        #δίνει default τιμές αν δεν υπάρχουν οι παράμετροι
        params.update({
            'kproj': int(kproj_match.group(1)) if kproj_match else 14,
            'M': int(m_match.group(1)) if m_match else 10,
            'probes': int(probes_match.group(1)) if probes_match else 2,
            'w': float(w_match.group(1)) if w_match else 4.0
        })

    #εξαγωγή παραμέτρων για IVFFlat αλγόριθμο
    elif 'ivfflat' in output_lower or 'flat' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        
        #δίνει default τιμές αν δεν υπάρχουν οι παράμετροι
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5
        })

    #εξαγωγή παραμέτρων για IVFPQ αλγόριθμο
    elif 'ivfpq' in output_lower or 'pq' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        nbits_match = re.search(r'nbits?(\d+)', output_lower)

        #δίνει default τιμές αν δεν υπάρχουν οι παράμετροι
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5,
            'M': int(m_match.group(1)) if m_match else 16,
            'nbits': int(nbits_match.group(1)) if nbits_match else 8
        })

    return params   #επιστροφή του λεξικού με τις παραμέτρους


#συνάτρηση που δημιουργεί πίνακα σύγκρισης για όλους τους αλγορίθμους
def create_comparison_table(df):
    comparison_data = []    #λίστα για αποθήκευση δεδομένων σύγκρισης 
    
    for _, row in df.iterrows():    #επανάληψη για κάθε γραμμή του dataframe
        #δημιουργία περιγραφής παραμέτρων ανά αλγόριθμο
        if row['Algorithm'] == 'LSH':
            params_desc = f"k={row.get('k', 4)}, L={row.get('L', 5)}, w={row.get('w', 4.0)}"
        elif row['Algorithm'] == 'Hypercube':
            params_desc = f"kproj={row.get('kproj', 14)}, M={row.get('M', 10)}, probes={row.get('probes', 2)}"
        elif row['Algorithm'] == 'IVFFlat':
            params_desc = f"kclusters={row.get('kclusters', 50)}, nprobe={row.get('nprobe', 5)}"
        elif row['Algorithm'] == 'IVFPQ':
            params_desc = f"kclusters={row.get('kclusters', 50)}, nprobe={row.get('nprobe', 5)}, M={row.get('M', 16)}, nbits={row.get('nbits', 8)}"
        else:
            params_desc = "Default"
        
        #προσθήκη λεξικού με τα δεδομένα σύγκρισης στη λίστα
        comparison_data.append({
            'Algorithm': row['Algorithm'],                  #όνομα αλγορίθμου
            'Parameters': params_desc,                      #περιγραφή παραμέτρων
            'Dataset': row['Dataset'],                      #dataset που χρησιμοποιήθηκε
            'Avg AF': row['Avg_AF'],                        #μέσος όρος AF
            'Recall@N': row['Avg_Recall'],                  #μέσος όρος Recall@N
            'QPS': row['QPS'],                              #QPS
            'Avg Approx Time (s)': row['Avg_Approx_Time'],  #μέσος χρόνος προσέγγισης 
            'Avg Exact Time (s)': row['Avg_Exact_Time'],    #μέσος ακριβής χρόνος
            'File': row['File']                             #όνομα αρχείου προέλευσης
        })
    
    return pd.DataFrame(comparison_data)    #δημιουργία και επιστροφή dataframe από τη λ΄ίστα 


def main():
    output_files = glob.glob("sift_output/*.dat")   #βρίσκει όλα τα output files στο φέκλο που θέλουμε sift_output για sift και mnist_output για mnist 
    all_final_metrics = []                          #λίστα για αποθήκευση όλων των μετρικών
    
    for file in output_files:                       #επεξεργασία κάθε αρχείου output
        print(f"Processing {file}...")
        final_metrics = parse_final_metrics(file)   #εξαγωγή μετρικών
        if final_metrics:                           #αν η εξαγωγή ήταν επιτυχής προσθήκη στη λίστα
            all_final_metrics.append(final_metrics)
    
    if not all_final_metrics:   #έλεγχος αν βρέθηκαν αποτελέσματα
        print("No results found!")
        return None
    
    df = pd.DataFrame(all_final_metrics)    #δημιουργία dataframe από όλες τις μετρικές 

    df.to_csv('final_metrics_comparison.csv', index=False)      #αποθήκευση τελικών μετρικ΄ων σε csv αρχείο
    print(f"Saved {len(df)} final metrics to final_metrics_comparison.csv")

    comparison_df = create_comparison_table(df)     #δημιουργία πίνακα σύγκρισης 
    
    #εκτύπωση κεφαλίδας για τον πίνακα σύγκρισης
    print("\n" + "="*80)
    print("FINAL METRICS COMPARISON TABLE")
    print("="*80)
    
    #ρύθμιση επιλογών display για pandas
    pd.set_option('display.max_columns', None)  #εμφάνιση όλων των στηλών
    pd.set_option('display.width', 1000)        #πλάτος display
    
    print(comparison_df.to_string(index=False)) #εκτύπωση του πίνακα σύγκρισης
    
    #αποθήκευση πίνακα σύγκρισης σε csv αρχείο
    comparison_df.to_csv('algorithm_comparison_table.csv', index=False)
    print(f"\n✅ Saved comparison table to algorithm_comparison_table.csv")
    
    return comparison_df    #επιστροφή του dataframe

#΄έλεγχος αν το script τρέχει απευθείας και κλήση της main συνάρτησης 
if __name__ == "__main__":
    df = main()