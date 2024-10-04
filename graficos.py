import pandas as pd

# Ruta del archivo TSV
path = 'C:/Users/franc/OneDrive/Escritorio/logt1/experiments_data/data.tsv'

# Leer el archivo TSV y convertirlo a un DataFrame
data = pd.read_csv(path, sep='\t')

# Mostrar las primeras filas del DataFrame
print(data.head())