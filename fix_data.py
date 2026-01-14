
import zipfile
import os

FILE = 'weather_data.csv'

def fix_file():
    if not os.path.exists(FILE):
        print("File not found.")
        return

    # Check for Zip signature
    with open(FILE, 'rb') as f:
        header = f.read(4)
    
    if header.startswith(b'PK'):
        print("[INFO] Detected ZIP/Excel signature.")
        
        try:
            with zipfile.ZipFile(FILE, 'r') as z:
                names = z.namelist()
                print(f"[INFO] Archive contents: {names}")
                
                # Check for Excel structure
                if 'xl/workbook.xml' in names or any(n.startswith('xl/') for n in names):
                    print("[INFO] Identified as Excel (.xlsx) file.")
                    try:
                        import pandas as pd
                        df = pd.read_excel(FILE)
                        df.to_csv('weather_data_fixed.csv', index=False)
                        print("[SUCCESS] Converted Excel to 'weather_data_fixed.csv'.")
                        
                        # Backup original and rename new
                        os.rename(FILE, 'weather_data_original.xlsx')
                        os.rename('weather_data_fixed.csv', FILE)
                        print("[SUCCESS] Replaced 'weather_data.csv' with converted plain text file.")
                        
                    except ImportError:
                        print("[ERROR] 'pandas' library not found. Cannot convert Excel file automatically.")
                        print("Please save your Excel file as CSV manually.")
                    except Exception as e:
                        print(f"[ERROR] Failed to convert: {e}")
                
                # Check for CSV inside Zip
                elif any(n.endswith('.csv') for n in names):
                    csv_file = next(n for n in names if n.endswith('.csv'))
                    print(f"[INFO] Found CSV inside zip: {csv_file}")
                    z.extract(csv_file)
                    
                    # Rename extracted file to weather_data.csv
                    os.rename(FILE, 'weather_data_backup.zip')
                    if os.path.exists(csv_file): # If extracted to current dir
                         if csv_file != FILE:
                             if os.path.exists(FILE): os.remove(FILE)
                             os.rename(csv_file, FILE)
                    print("[SUCCESS] Extracted CSV from zip.")
                    
                else:
                    print("[WARNING] Unknown zip content. Cannot automatically fix.")
                    
        except zipfile.BadZipFile:
            print("[ERROR] Corrupted zip file.")
    else:
        print("[INFO] File does not start with PK. It might already be a text file or another format.")
        # Print first few chars
        with open(FILE, 'r', errors='ignore') as f:
            print("First line:", f.readline())

if __name__ == "__main__":
    fix_file()
