# SCRIPT ΕΞΑΓΩΓΗΣ ΜΕΤΡΙΚΩΝ ΑΠΟ OUTPUT FILES

import os
import pandas as pd
import re
import glob

def parse_final_metrics(output_file):
    """Εξάγει μόνο τις ΤΕΛΙΚΕΣ ΜΕΤΡΙΚΕΣ για κάθε εκτέλεση"""
    try:
        with open(output_file, 'r', encoding='utf-8') as file:
            content = file.read()

        # Εύρεση αλγόριθμου
        algorithm_match = re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content)
        algorithm = algorithm_match.group(1) if algorithm_match else "Unknown"

        # Εξαγωγή FINAL METRICS
        final_metrics = {
            'Algorithm': algorithm,
            'File': os.path.basename(output_file),
            'Dataset': 'MNIST' if 'mnist' in output_file.lower() else 'SIFT',
            'Avg_AF': extract_final_metric(r'Total Average AF: (\d+\.\d+)', content),
            'Avg_Recall': extract_final_metric(r'Total Average Recall@N: (\d+\.\d+)', content),
            'QPS': extract_final_metric(r'QPS: (\d+\.\d+)', content),
            'Avg_Approx_Time': extract_final_metric(r'Average Approximate Time: (\d+\.\d+)', content),
            'Avg_Exact_Time': extract_final_metric(r'Average Exact Time: (\d+\.\d+)', content),
        }
        
        # Εξαγωγή παραμέτρων
        final_metrics.update(extract_params(output_file))
        
        print(f"✅ Extracted final metrics from {output_file}")
        return final_metrics

    except Exception as ex:
        print(f"❌ Error parsing file {output_file}: {ex}")
        return None

def extract_final_metric(pattern, content):
    """Εξάγει την τιμή από το pattern για FINAL METRICS"""
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
        w_match = re.search(r'w([\d\.]+)', output_lower)
        params.update({
            'k': int(k_match.group(1)) if k_match else 4,
            'L': int(L_match.group(1)) if L_match else 5,
            'w': float(w_match.group(1)) if w_match else 4.0
        })

    # Hypercube params
    elif 'hypercube' in output_lower or 'hyp' in output_lower:
        kproj_match = re.search(r'kproj(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        probes_match = re.search(r'probes?(\d+)', output_lower)
        w_match = re.search(r'w([\d\.]+)', output_lower)
        params.update({
            'kproj': int(kproj_match.group(1)) if kproj_match else 14,
            'M': int(m_match.group(1)) if m_match else 10,
            'probes': int(probes_match.group(1)) if probes_match else 2,
            'w': float(w_match.group(1)) if w_match else 4.0
        })

    # IVFFlat params
    elif 'ivfflat' in output_lower or 'flat' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5
        })

    # IVFPQ params
    elif 'ivfpq' in output_lower or 'pq' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        nbits_match = re.search(r'nbits?(\d+)', output_lower)
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5,
            'M': int(m_match.group(1)) if m_match else 16,
            'nbits': int(nbits_match.group(1)) if nbits_match else 8
        })

    return params

def create_comparison_table(df):
    """Δημιουργεί πίνακα σύγκρισης για όλους τους αλγορίθμους"""
    comparison_data = []
    
    for _, row in df.iterrows():
        # Δημιουργία περιγραφής παραμέτρων ανά αλγόριθμο
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
        
        comparison_data.append({
            'Algorithm': row['Algorithm'],
            'Parameters': params_desc,
            'Dataset': row['Dataset'],
            'Avg AF': row['Avg_AF'],
            'Recall@N': row['Avg_Recall'],
            'QPS': row['QPS'],
            'Avg Approx Time (s)': row['Avg_Approx_Time'],
            'Avg Exact Time (s)': row['Avg_Exact_Time'],
            'File': row['File']
        })
    
    return pd.DataFrame(comparison_data)

def main():
    """Βασική συνάρτηση επεξεργασίας αποτελεσμάτων"""
    
    # Βρίσκει όλα τα output files
    output_files = glob.glob("*.dat") + glob.glob("*.txt")  # Προσθήκη .txt αν χρειαστεί
    all_final_metrics = []
    
    for file in output_files:
        print(f"📖 Processing {file}...")
        final_metrics = parse_final_metrics(file)
        if final_metrics:
            all_final_metrics.append(final_metrics)
    
    if not all_final_metrics:
        print("❌ No results found!")
        return None
    
    # Create dataframe
    df = pd.DataFrame(all_final_metrics)

    # Save final metrics to CSV
    df.to_csv('final_metrics_comparison.csv', index=False)
    print(f"✅ Saved {len(df)} final metrics to final_metrics_comparison.csv")

    # Create and display comparison table
    comparison_df = create_comparison_table(df)
    
    print("\n" + "="*80)
    print("📊 FINAL METRICS COMPARISON TABLE")
    print("="*80)
    
    # Εμφάνιση πίνακα σύγκρισης
    pd.set_option('display.max_columns', None)
    pd.set_option('display.width', 1000)
    
    print(comparison_df.to_string(index=False))
    
    # Αποθήκευση πίνακα σύγκρισης
    comparison_df.to_csv('algorithm_comparison_table.csv', index=False)
    print(f"\n✅ Saved comparison table to algorithm_comparison_table.csv")
    
    return comparison_df

if __name__ == "__main__":
    df = main()