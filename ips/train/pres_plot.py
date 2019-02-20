
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats


def f1_plot(results,title=None,save=None,
	req=0.95,level=0.95,metric='val_f1'):
	"""
	f1_plot() generates a plot for average f1 score over the course of training 
	a neural network several times. The plot includes raw data as well as average 
	curve and error bars

	Inputs:
	results: dictionary of metrics resulting from repeated trainings of a network
	title: a string to be included as a plot title
	save: a name for the plot to be saved to a png file with
	req: Average F1 score requirement to be displayed on the plot
	level: Statistical confidence level to calculate confidence intervals with
	metric: change this to plot other keys of the results input
	"""
	M, epochs = np.array(results[metric]).shape
	mean_f1 = np.mean(results[metric],axis=0)

	# Amount of uncertainty based on confidence level
	std_f1 = np.std(results[metric],axis=0)
	unc_f1 = stats.t.ppf(1/2+level/2,df=M-1) * std_f1/np.sqrt(M)
	eax = np.arange(epochs)+1


	fig,ax = plt.subplots(1,1,figsize=(12,6),facecolor='black')

	if title:
		plt.title(title,fontsize=24,color='white')

	plt.xlabel('Rounds of Training (Epochs)',fontsize=18)
	plt.ylabel('F1 Score',fontsize=18)
	ax.xaxis.label.set_color('white')
	ax.yaxis.label.set_color('white')
	ax.tick_params(colors='white')
	ax.set_facecolor('xkcd:charcoal')
	meas = plt.plot(eax,np.transpose(results[metric]),'o',
	  markersize=8,
	  alpha=3/M,
	  color='white')
	# Line for the mean
	# plt.plot(eax,mean_f1,
	#   linewidth=2,
	#   color='w',
	#   label='Mean F1 Score')
	plt.errorbar(eax,mean_f1,
		yerr=unc_f1,
		fmt='.-',
		linewidth=2,
		elinewidth=4,
		capsize=4,
		color='w',
		ecolor='xkcd:cyan',
		label=f'Mean F1 Score with {100*level:.0f}% Confidence Intervals')
	plt.axhline(y=req,
	  linestyle='--',
	  color='xkcd:pale green',
	  linewidth=2,
	  label='F1 Score Requirement')
	if metric.find('f1')>1 or metric.find('acc')>1 :
		plt.axis((0,epochs+1,0.8,1))
	ax.set_xticks(np.arange(0,epochs+1,2))
	xbox = [0,epochs+1,epochs+1,0]
	ybox = [1,1,req,req]
	plt.fill(xbox,ybox,
		alpha = 0.15,
		color='xkcd:light green',
		label=f'Region for Requirement F1 >= {req:.2f}')
	plt.grid(True,color='grey')
	leg = plt.legend(facecolor='grey',fontsize=14)
	plt.setp(leg.get_texts(),color='w')
	# save the fig is we want to
	if save:
		plt.savefig(save+'.png',
			facecolor='black',edgecolor='black')

	plt.show()


# plt.plot(np.arange(epochs),mean_f1+unc_f1,'-',
#   color='xkcd:green',
#   linewidth=2,
#   label=f'{100*level:.0f}% Confidence Interval Upper Limit')
# plt.plot(np.arange(epochs),mean_f1-unc_f1,'-',
#   color='xkcd:red',
#   linewidth=2,
#   label=f'{100*level:.0f}% Confidence Interval Lower Limit')

# plt.xlim([0,epochs-1])
# # plt.ylim([0,1])
# plt.grid(True)
# plt.xlabel('Training Epochs',
#   fontsize=16)
# plt.ylabel('Mean F1 Score',
#   fontsize=16)
# plt.title(
#   f'Mean F1 Scores at Each Epoch\n' \
#   f'Feature Detector:{args["model"]}, ' \
#   f'Trained on {N_im} Images{(", Resampled" if args["resampling"] else "")}',
#   fontsize=20)
# h,l = plt.gca().get_legend_handles_labels()
# h1 = [meas[0]]+h
# l1 = ['F1 Score Measuremets']+l
# plt.legend(h1,l1,fontsize=12,loc='best')
# # Save this figure to a png also
# savefile = f'{plotdir}mean_f1_{args["model"]}_e{epochs}_n{N_im}_m{M}.png'
# i=1
# while os.path.isfile(savefile):
#   savefile = f'{plotdir}mean_f1_{args["model"]}_e{epochs}_n{N_im}_m{M}_{i}.png'
#   i+=1

# plt.savefig(savefile)
# print(f'[INFO] Mean F1 chart saved to {savefile}')
# plt.show()

def plot_errors(model,ground_truth,test_inputs,modelname=None,mono=False):
  # ground_truth = test_labels
	predictions = model.predict(test_inputs).reshape(ground_truth.shape)
	#find where the errors in the testing data are
	errors = np.where((predictions>0.5) != ground_truth)[0]
	print("Number of Errors = {}/{}".format(len(errors),len(ground_truth)))
	# Create this list thing for plotting examples

	for i in errors:
		# Print the classification and prediction for this case
		print(f'Image does {"" if ground_truth[i] else "not"} contain aurora,'
		      f' Probability: {predictions[i]}')
		# Show the image for reference
		if mono:
			plt.imshow(test_inputs[i][:,:,1])
		else:
			plt.imshow(test_inputs[i])
		plt.title(f'Predicted Aurora Probability = {100*predictions[i]:.2f}%')
		plt.grid(False)
		if modelname:
			plt.savefig(f'plots/ex/error_{modelname}_{i}.png')
		plt.show()